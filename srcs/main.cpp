/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ellanglo <ellanglo@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 16:22:15 by ellanglo          #+#    #+#             */
/*   Updated: 2025/09/15 18:41:16 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <Server.hpp>

int main(int ac, char **av) 
{
	(void)ac;
	Server server(atoi(av[1]), av[2]);
	server.launch();
    return 0;
}
