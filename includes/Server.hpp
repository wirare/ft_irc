/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ellanglo <ellanglo@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 18:37:21 by ellanglo          #+#    #+#             */
/*   Updated: 2025/12/10 19:19:19 by ellanglo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#pragma once
#include <RPL_list.hpp>
#include <auto.hpp>
#include <cassert>
#include <cerrno>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <map>
#include <ostream>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <Error.hpp>
#include <Client.hpp>
#include <Message.hpp>
#include <ctime>
#include <ErrorCode.hpp>
#include <Channel.hpp>
#include <Commands.hpp>
#include <Signals.hpp>
#include <fcntl.h>

#define MAX_CLIENT 128
#define SERVER
#include <Send.hpp>

class Server
{
public:
	Server() {};
	~Server() 
	{
		for (auto it = channelMap.begin(); it != channelMap.end(); it++)
			delete it->second;
		for (auto it = clientMap.begin(); it != clientMap.end(); it++)
			delete it->second;
		close(sock_fd);
	};
	void init(int _port, std::string _password)
	{
		name = "localhost";
		port = _port;
		password = _password;
		try
		{
			setup_signals();
			open_socket();
			bind_port();
			listen_socket();
			create_epoll();
			time(&startTime);
		}
		catch (std::exception &e)
		{
			if (sock_fd != 1)
				close(sock_fd);
			std::cout << "Error on Server setup : " << e.what() << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	void launch()
	{
		int nfds;
		while (1)
		{
			if (g_stop == 1)
				return;
			nfds = epoll_wait(epoll_fd, events, MAX_CLIENT, -1);
			if (nfds == -1)
				throw EPOLL_WAIT_FAILURE;
			for (int n = 0; n < nfds; n++)
			{
				if (events[n].data.fd == sock_fd)
				{
					try { handle_connect(); }
					catch (std::exception &e) { throw e; }
				}
				else
					handle_message(n);
			}
		}
	}

	void handle_connect()
	{
		int conn_sock = accept(sock_fd, reinterpret_cast<sockaddr*>(&addr), &addrlen);
		if (conn_sock == -1)
			throw CANT_ACCEPT_CONNECTION;
		ev.events = EPOLLIN;
		ev.data.fd = conn_sock;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_sock, &ev) == -1)
			throw EPOLL_CTL_ADD_FAILURE;
		Client *client = new Client(conn_sock);
		clientMap.insert(std::pair<int, Client*>(conn_sock, client));
	}

	void handle_message(int n)
	{
		char buf[512];
		int fd = events[n].data.fd;
		Client *client = clientMap.at(fd);

		int count = recv(client->getFd(), buf, sizeof(buf), 0);
		if (count <= 0)
		{
			partialBuffers.erase(fd);
			deleteClient(clientMap[fd]);
			return;
		}

		std::string &leftover = partialBuffers[fd];
		leftover.append(buf, count);

		const size_t MAX_LEFTOVER = 8192;
		if (leftover.size() > MAX_LEFTOVER) 
		{
			std::cerr << "Client " << fd << " leftover too large, closing connection\n";
			partialBuffers.erase(fd);
			deleteClient(clientMap[fd]);
			return;
		}

		size_t pos;
		while ((pos = leftover.find('\n')) != std::string::npos) 
		{
			std::string line = leftover.substr(0, pos);
			leftover.erase(0, pos + 1);
			if (!line.empty() && line[line.size() - 1] == '\r') 
				line.erase(line.size() - 1);
			if (line.empty())
				continue;
			std::cout << "Client number " << fd << " sent : " << line << std::endl;
			IrcMessage msg(line);
			executeCommand(msg, client);
		}
	}

	void open_socket()
	{
		sock_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (sock_fd == -1)
			throw CANT_OPEN_SOCKET;

		int opt = 1;
		setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	}

	void bind_port()
	{
		std::memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_port = htons(port);
		addrlen = sizeof(addr);
		if (bind(sock_fd, reinterpret_cast<sockaddr*>(&addr), addrlen) == -1)
			throw CANT_BIND_TO_PORT;
	}

	void listen_socket()
	{
		if (listen(sock_fd, SOMAXCONN) == -1)
			throw CANT_LISTEN_SOCKET;
	}

	void create_epoll()
	{
		epoll_fd = epoll_create(1);
		if (epoll_fd == -1)
			throw EPOLL_CTL_ADD_FAILURE;
		ev.events = EPOLLIN;
		ev.data.fd = sock_fd;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &ev) == -1)
			throw EPOLL_CTL_ADD_FAILURE;
	}

	std::string buildMessage(const char *fmt, ...)
	{
		va_list args;
		va_start(args, fmt);

		std::ostringstream oss;

		bool	noNl = false;
		for (const char* p = fmt; *p && !noNl; p++)
		{
			switch (*p)
			{
				case 'o':
				{
					oss << ":" << name << " ";
					break;
				}
				case 's':
				{
					const std::string &s = va_arg(args, const char*);
					oss << s;
					break;
				}
				case 'd':
				{
					int i = va_arg(args, int);
					oss << i;
					break;
				}
				case 'l':
				{
					long l = va_arg(args, long);
					oss << l;
					break;
				}
				case 'n':
				{
					noNl = true;
					break;
				}
				case 'c':
				{
					oss << ":";
					break;
				}
			}
		}

		va_end(args);
		if (!noNl) oss << "\r\n";
		return oss.str();
	}

	inline std::string sendMessage(int fd, const std::string &msg)
	{
		send(fd, msg.c_str(), msg.size(), 0);
		return msg;
	}

	inline void sendError(int err, int fd, const std::string& err_param)
	{
		SEND("odssss", err, " ", err_param.c_str(), " :", getErrMsg(err).c_str());
	}

	inline void sendSuccessfulRegister(int fd)
	{
		Client *client = clientMap.at(fd);
		const std::string str_nick = client->getNick();
		const char *nick = str_nick.c_str();
		SEND("odssssssss", RPL_WELCOME, " ", nick, " :Welcome to the IRC network ", nick, "!", client->getUsername().c_str(), "@", client->getHostname().c_str());
		SEND("odssssss", RPL_YOURHOST, " ", nick, " :Your host is ", name.c_str(), ", running version", " 1.0");
		SEND("odssssn", RPL_CREATED, " ", nick, " :This server was created ", ctime(&startTime));
		SEND("odsssssss", RPL_MYINFO, " ", nick, " ", name.c_str(), " 1.0", " iow", " irsk");
	}

	inline std::map<int, Client*>& getClientMap() { return clientMap; }
	inline std::string getPassword() const { return password; }
	inline Channel *getChannel(const std::string &name)
	{
		auto chan = channelMap.find(name);
		if (chan != channelMap.end())
			return chan->second;
		return NULL;
	}

	inline Channel* createChannel(const std::string &name) 
	{
		Channel *chan = new Channel(name);
		channelMap[name] = chan;
		return chan;
	}

	inline void deleteChannel(Channel *chan)
	{
		channelMap.erase(chan->getName());
		delete chan;
	}

	inline void deleteClient(Client *client)
	{
		clientMap.erase(client->getFd());
		delete client;
	}

	inline std::vector<Channel *> getClientChannel(const Client *client)
	{
		std::vector<Channel *> channels;
		for (auto it = channelMap.begin(); it != channelMap.end(); it++)
		{
			if (it->second->hasClient(client))
				channels.push_back(it->second);
		}
		return channels;
	}
	inline int getChannelNumber() const { return channelMap.size(); }
	inline Client *getClient(const std::string &nick)
	{
		for (auto it = clientMap.begin(); it != clientMap.end(); it++)
		{
			if (it->second->getNick() == nick)
				return it->second;
		}
		return NULL;
	}

private:
	std::string name;
	int sock_fd;
	int epoll_fd;
	int	port;
	std::string password;
	sockaddr_in addr;
	socklen_t addrlen;
	struct epoll_event ev, events[MAX_CLIENT];
	std::map<int, Client*> clientMap;
	std::map<std::string, Channel*> channelMap;
	time_t startTime;
	std::map<int, std::string> partialBuffers;
};
#undef SERVER

extern Server server;
