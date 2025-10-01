/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ellanglo <ellanglo@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 18:37:21 by ellanglo          #+#    #+#             */
/*   Updated: 2025/10/01 18:09:27 by ellanglo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#pragma once
#include "auto.hpp"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <map>
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
#include <Channel.hpp>

#define MAX_CLIENT 128
#define SERVER
#include <Send.hpp>

class Server
{
public:
	Server() {};
	Server(int port, std::string password): name("localhost"), port(port), password(password)
	{
		try
		{
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
		std::cout << "Password : " << password << std::endl;
		int nfds;
		while (1)
		{
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
		ev.events = EPOLLIN | EPOLLET;
		ev.data.fd = conn_sock;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_sock, &ev) == -1)
			throw EPOLL_CTL_ADD_FAILURE;
		Client client(conn_sock);
		clientMap.insert(std::pair<int, Client>(conn_sock, client));
	}

	void handle_message(int n)
	{
		char buf[512];
		Client &client = clientMap.at(events[n].data.fd);
		int count = recv(client.getFd(), buf, sizeof(buf) - 1, 0);
		if (count <= 0) 
		{
			close(client.getFd());
			return ;
		}
		buf[count] = '\0';
		if (!*buf)
			return;
		std::cout << "Client number " << client.getFd() << " sent : " << buf;
		std::vector<std::string> commands = StringHelper::split(buf, '\n');
		for (auto it = commands.begin(); it != commands.end(); ++it)
		{
			IrcMessage msg(it->data());
			if (msg.id != UNKNOWN)
				executeCommand(msg, client);
			else
				client.forwardMessage(it->data());
		}
	}

	void open_socket()
	{
		sock_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
		int opt = 1;
		if (sock_fd == 1)
			throw CANT_OPEN_SOCKET;
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

	inline void sendMessage(int fd, const std::string &msg)
	{
		send(fd, msg.c_str(), msg.size(), 0);
	}

	inline void sendError(int err, int fd)
	{
		SEND("odss", err, " * : ", getErrMsg(err).c_str());
	}

	inline void sendSuccessfulRegister(int fd)
	{
		Client &client = clientMap.at(fd);
		std::cout << client;
		const std::string str_nick = client.getNick();
		const char *nick = str_nick.c_str();
		SEND("osssssss", "001 ", nick, " :Welcome to the IRC network ", nick, "!", client.getUsername().c_str(), name.c_str());
		SEND("ossssss", "002 ", nick, " :Your host is ", name.c_str(), ", running version", " 1.0");
		SEND("ossssn", "003 ", nick, " :This server was created ", ctime(&startTime));
		SEND("osssssss", "004 ", nick, " ", name.c_str(), " 1.0", " iow", " irsk");
	}

	inline std::map<int, Client>& getClientMap() { return clientMap; }
	inline std::string getPassword() const { return password; }
	inline bool channelExist(const std::string &name)
	{
		for (auto it = channelList.begin(); it != channelList.end(); it++)
		{
			if (it->getName() == name)
				return true;
		}
		return false;
	}
	inline Channel getChannel(const std::string &name)
	{
		for (auto it = channelList.begin(); it != channelList.end(); it++)
		{
			if (it->getName() == name)
				return *it;
		}
		__builtin_unreachable();
	}
	inline void addChannel(Channel &channel)
	{
		channelList.push_back(channel);
	}
	inline std::vector<Channel> getClientChannel(const Client &client)
	{
		std::vector<Channel> channels;
		for (auto it = channelList.begin(); it != channelList.end(); it++)
		{
			if (it->hasClient(client))
				channels.push_back(*it);
		}
		return channels;
	}
	inline int getChannelNumber() const { return channelList.size(); }

private:
	std::string name;
	int sock_fd;
	int epoll_fd;
	int	port;
	std::string password;
	sockaddr_in addr;
	socklen_t addrlen;
	struct epoll_event ev, events[MAX_CLIENT];
	std::map<int, Client> clientMap;
	std::vector<Channel> channelList;
	time_t startTime;
};
#undef SERVER

extern Server server;
