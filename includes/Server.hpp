/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ellanglo <ellanglo@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 18:37:21 by ellanglo          #+#    #+#             */
/*   Updated: 2025/09/12 01:29:28 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#pragma once
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/epoll.h>
#include "Error.hpp"

class Server
{
public:
	Server(int port, char *password): port(port), password(password)
	{
		try
		{
			open_socket();
			bind_port();
			listen_socket();
			create_epoll();
		}
		catch (std::exception &e)
		{
			std::cout << "Error on Server setup : " << e.what() << std::endl;
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
		sockaddr_in addr;

		std::memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_port = htons(port);
		socklen_t addrlen = sizeof(addr);
		if (bind(sock_fd, reinterpret_cast<sockaddr*>(&addr), addrlen) == -1)
		{
			close(sock_fd);
			throw CANT_BIND_TO_PORT;
		}
	}

	void listen_socket()
	{
		if (listen(sock_fd, SOMAXCONN) == -1)
		{
			close(sock_fd);
			throw CANT_LISTEN_SOCKET;
		}
	}

	void create_epoll()
	{
		epoll_fd = epoll_create(1);

		ev.events = EPOLLIN;
		ev.data.fd = sock_fd;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &ev) == -1)
		{
			close(sock_fd);
			throw EPOLL_CTL_ADD_FAILURE;
		}
	}

private:
	int sock_fd;
	int epoll_fd;
	int	port;
	char *password;
	struct epoll_event ev, events[16];
};
