/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wirare <wirare@42angouleme.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 19:22:04 by wirare            #+#    #+#             */
/*   Updated: 2025/10/06 12:59:13 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <StringHelper.hpp>
#include <Message.hpp>
#include <Channel.hpp>
#include <Server.hpp>
#define CHANNEL
#include <Send.hpp>

Channel::Channel(const std::string &name): Topic(""), Name(name), channelModes(0)
{
	Id = server.getChannelNumber();
}

void Channel::addClient(Client *client, ClientState state, const std::string &key)
{
	if (hasMode(USER_LIMIT) && clientMap.size() >= UserLimit)
		SEND_ERR(471);
	if (hasMode(CHANNEL_KEY) && key != Key)
		SEND_ERR(475);
	if (hasMode(INVITE_ONLY))
	{
		auto clientState = clientMap.find(client);
		if (clientState == clientMap.end() || clientState->second != INVITED)
			SEND_ERR(473);
	}
	clientMap[client] = state;
	successfulJoin(client);
}

void Channel::successfulJoin(Client *client)
{
	SEND("csss", client->getUsername().c_str(), " JOIN ", Name.c_str());
	SEND("sssss", client->getUsername().c_str(), " ", Name.c_str(), ":", Topic.c_str());
	executeCommandInternal(NAMES, StringHelper::makeVector("NAMES ", Name), client);
}

void Channel::recvMessage(const Client *sender, const std::string &msg) const
{
	for (auto _client = clientMap.begin(); _client != clientMap.end(); _client++)
	{
		const Client *client = _client->first;
		if (client != sender)
			SEND("csssss", sender->getUsername().c_str(), " PRIVMSG ", Name.c_str(), " ", msg.c_str());
	}
}
#undef CHANNEL
