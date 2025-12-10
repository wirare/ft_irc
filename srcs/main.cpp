/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ellanglo <ellanglo@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 16:22:15 by ellanglo          #+#    #+#             */
/*   Updated: 2025/12/10 19:21:20 by ellanglo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <Server.hpp>
#include <cstdlib>
#include <cctype>
#include <exception>
#include <iostream>

Server server;

int main(int ac, char **av) 
{
	if (ac != 3 || !std::atoi(av[1]))
	{
		std::cerr << "Wrong argument\n";
		return 1;
	}
	server.init(std::atoi(av[1]), av[2]);
	try
	{
		pid_t pid = fork();
		if (pid == 0)
		{
		    execl("./bot/bot", "./bot/bot", av[1], av[2], NULL);
		    perror("execl");
		    exit(1);
		}
		server.launch();
	}
	catch (std::exception &e)
	{
		if (g_stop)
			return 0;
		else
			std::cerr << e.what() << std::endl;
	}
    return 0;
}
