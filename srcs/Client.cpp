/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ellanglo <ellanglo@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 17:08:15 by ellanglo          #+#    #+#             */
/*   Updated: 2025/10/01 17:54:20 by ellanglo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <auto.hpp>
#include <Channel.hpp>
#include <Client.hpp>
#include <Server.hpp>
#include <string>
#define CLIENT
#include <Send.hpp>

void Client::forwardMessage(const std::string &msg) const
{
	for (auto channel = channelList.begin(); channel != channelList.end(); channel++)
	{
		auto clientMap = channel->getClientMap();
		for (auto it = clientMap.begin(); it != clientMap.end(); it++)
			SEND(it->first.getFd(), "s", msg.c_str());
	}
}
#undef CLIENT
