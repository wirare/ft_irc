/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ellanglo <ellanglo@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 17:08:15 by ellanglo          #+#    #+#             */
/*   Updated: 2025/10/02 00:34:58 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <auto.hpp>
#include <Channel.hpp>
#include <Client.hpp>
#include <Server.hpp>
#include <string>
#define CLIENT
#include <Send.hpp>

void Client::recvMessage(const Client &client, const std::string &msg) const
{
	std::cout << "Client " << fd << " try to send to client " << client.getFd() << " : " << msg << "\n";
	SEND("csssss", client.getNick().c_str(), " PRIVMSG ", Nick.c_str(), " ", msg.c_str());
}

#undef CLIENT
