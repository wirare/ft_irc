/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ellanglo <ellanglo@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 16:22:15 by ellanglo          #+#    #+#             */
/*   Updated: 2025/09/11 19:06:13 by ellanglo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/epoll.h>
#include "includes/Server.hpp"

int main(int ac, char **av) 
{
	if (ac != 3)
		return 1;

    int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	int opt = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (sockfd == -1) 
	{
        perror("socket");
        return 1;
    }

    sockaddr_in addr;

    std::memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;        
    addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(atoi(av[1]));
	socklen_t	addrlen = sizeof(addr);

    if (bind(sockfd, reinterpret_cast<sockaddr*>(&addr), addrlen) == -1) 
	{
        perror("bind");
        close(sockfd);
        return 1;
	}

	if (listen(sockfd, SOMAXCONN) == -1) {
		perror("listen");
		close(sockfd);
		return 1;
	}

	int epoll_fd = epoll_create(1);		
	struct epoll_event ev, events[16];

	ev.events = EPOLLIN;
	ev.data.fd = sockfd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &ev) == -1)
	{
		perror("epoll_ctl: listen_sock");
		exit(EXIT_FAILURE);
	}

	int nfds;
	while (1)
	{
		nfds = epoll_wait(epoll_fd, events, 16, -1);
		if (nfds == -1)
		{
			perror("epoll_wait");
			exit(EXIT_FAILURE);
		}

		for (int n = 0; n < nfds; n++)
		{
			if (events[n].data.fd == sockfd)
			{
				int conn_sock = accept(sockfd, (struct sockaddr *)&addr, &addrlen);
				if (conn_sock == -1)
				{
					perror("accept");
					exit(EXIT_FAILURE);
				}
				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = conn_sock;
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_sock, &ev) == -1)
				{
					perror("epoll_ctl: conn_sock");
					exit(EXIT_FAILURE);
				}
			}
		}
	}

    return 0;
}
