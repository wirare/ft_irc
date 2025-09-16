/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ellanglo <ellanglo@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 16:22:15 by ellanglo          #+#    #+#             */
/*   Updated: 2025/09/16 01:54:57 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <Server.hpp>

Server server;

int main(int ac, char **av) 
{
	(void)ac;
	server = Server(atoi(av[1]), av[2]);
	server.launch();
    return 0;
}
