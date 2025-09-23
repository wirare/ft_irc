/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ellanglo <ellanglo@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 18:37:21 by ellanglo          #+#    #+#             */
/*   Updated: 2025/09/23 20:25:32 by ellanglo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#pragma once
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

#define MAX_CLIENT 128

#define SEND(...) sendMessage(fd, buildMessage(__VA_ARGS__))

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
		client.setState(WAIT_PASS);
		clientMap.insert(std::pair<int, Client>(conn_sock, client));
		sendSuccessfulRegister(conn_sock);
	}

	std::vector<std::string> split_message(char* buf)
	{
		std::vector<std::string> tokens;
		std::string token;
		std::istringstream tokenStream(buf);
		while (std::getline(tokenStream, token, '\n'))
			tokens.push_back(token);
		return tokens;
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
		std::cout << "Client number " << client.getFd() << " sent : " << buf;
		buf[count] = '\0';
		if (!*buf)
			return;
		std::vector<std::string> commands = split_message(buf);
		for (std::vector<std::string>::iterator it = commands.begin(); it != commands.end(); ++it)
		{
			IrcMessage msg(it->data());
			executeCommand(msg, client);
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

		oss << ":" << name << " ";
		for (const char* p = fmt; *p; p++)
		{
			switch (*p)
			{
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
			}
		}

		va_end(args);
		oss << "\r\n";
		return oss.str();
	}

	inline void sendMessage(int fd, const std::string &msg)
	{
		send(fd, msg.c_str(), msg.size(), 0);
	}

	inline void sendError(int err, int fd)
	{
		SEND("dss", err, " * : ", getErrMsg(err).c_str());
	}

	inline void sendSuccessfulRegister(int fd)
	{
		Client client = clientMap.at(fd);
		const std::string str_nick = client.getNick();
		const char *nick = str_nick.c_str();
		SEND("ssssssss", "001 ", nick, " :Welcome to the IRC network ", nick, "!", client.getUsername().c_str(), name.c_str());
		SEND("ssssss", "002 ", nick, " :Your host is ", name.c_str(), " ,running version", " 1.0");
		SEND("ssss", "003 ", nick, " :This server was created ", ctime(&startTime));
		SEND("sssssss", "004 ", nick, " ", name.c_str(), " 1.0", " iow ", "irsk");
	}

	inline std::map<int, Client>& getClientMap() { return clientMap; }
	inline std::string getPassword() const { return password; }

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
	time_t startTime;
};

extern Server server;
