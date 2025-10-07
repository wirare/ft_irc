/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wirare <wirare@42angouleme.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 19:22:04 by wirare            #+#    #+#             */
/*   Updated: 2025/10/07 20:24:30 by ellanglo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <StringHelper.hpp>
#include <Message.hpp>
#include <Channel.hpp>
#include <Server.hpp>
#include <RPL_list.hpp>
#include <algorithm>
#include <ctime>
#define CHANNEL
#include <Send.hpp>

Channel::Channel(const std::string &name): Topic(""), Name(name), TopicTime(std::time(NULL)), channelModes(0)
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
	if (clientMap.size() == 0)
		TopicChanger = client->getNick();
	clientMap[client] = state;
	successfulJoin(client);
}

void Channel::successfulJoin(Client *client)
{
	std::string msg = SEND("csss", client->getUsername().c_str(), " JOIN ", Name.c_str());
	SEND("odssssss", RPL_TOPIC, " ", client->getUsername().c_str(), " ", Name.c_str(), " :", Topic.c_str());
	executeCommandInternal(NAMES, StringHelper::makeVector("NAMES ", Name), client);
	broadcast(msg, client);
}

void Channel::recvMessage(Client *sender, const std::string &msg) const
{
	std::string str(":");
	str += sender->getUsername() + " PRIVMSG " + Name + " " + msg;
	broadcast(str, sender);
}

void Channel::broadcast(const std::string &msg, Client *sender) const
{
	std::vector<Client *> vec;
	vec.push_back(sender);
	broadcast(msg, vec);
}

void Channel::broadcast(const std::string &msg, const std::vector<Client *> &exceptions) const
{
	for (auto _client = clientMap.begin(); _client != clientMap.end(); _client++)
	{
		const Client *client = _client->first;
		if (exceptions.size())
		{
			auto findClient = std::find(exceptions.begin(), exceptions.end(), client);
			if (findClient != exceptions.end())
				continue;
		}
		SEND("s", msg.c_str());
	}
}

#undef CHANNEL
