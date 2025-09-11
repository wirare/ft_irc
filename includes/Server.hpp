/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ellanglo <ellanglo@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 18:37:21 by ellanglo          #+#    #+#             */
/*   Updated: 2025/09/11 19:05:42 by ellanglo         ###   ########.fr       */
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
	Server(int port, char *password)
	{
		
	}

	void open_socket()
	{
		sock_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
		int opt = 1;
		if (sock_fd == 1)
			throw CANT_OPEN_SOCKET;
		setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	}

private:
	int sock_fd;
	int epoll_fd;
	int	port;
	char *password;
};
