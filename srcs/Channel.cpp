/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wirare <wirare@42angouleme.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 19:22:04 by wirare            #+#    #+#             */
/*   Updated: 2025/10/14 17:42:53 by ellanglo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <ErrorCode.hpp>
#include <StringHelper.hpp>
#include <Message.hpp>
#include <Channel.hpp>
#include <Server.hpp>
#include <RPL_list.hpp>
#include <algorithm>
#include <ctime>
#define CHANNEL
#include <Send.hpp>

Channel::Channel(const std::string &name): Topic(""), Name(name), CreationTime(std::time(NULL)), TopicTime(std::time(NULL)), channelModes(0)
{
	Id = server.getChannelNumber();
}

void Channel::addClient(Client *client, ClientState state, const std::string &key)
{
	if (hasMode(USER_LIMIT) && clientMap.size() >= UserLimit)
		SEND_ERR(ERR_CHANNELISFULL, _NICK, Name);
	if (hasMode(CHANNEL_KEY) && key != Key)
		SEND_ERR(ERR_BADCHANNELKEY, _NICK, Name);
	if (hasMode(INVITE_ONLY))
	{
		auto clientState = clientMap.find(client);
		if (clientState == clientMap.end() || clientState->second != INVITED)
			SEND_ERR(ERR_INVITEONLYCHAN, _NICK, Name);
	}
	if (clientMap.size() == 0)
		TopicChanger = client->getNick();
	clientMap[client] = state;
	successfulJoin(client);
}

void Channel::delClient(Client *client)
{
	clientMap.erase(client);
	if (clientMap.size() == 0)
		server.deleteChannel(this);
}

void Channel::successfulJoin(Client *client)
{
	std::string msg = SEND("csss", client->getNick().c_str(), " JOIN ", Name.c_str());
	SEND("odssssss", RPL_TOPIC, " ", client->getNick().c_str(), " ", Name.c_str(), " :", Topic.c_str());
	executeCommandInternal(NAMES, StringHelper::makeVector("NAMES ", Name), client);
	broadcast(msg, client);
}

void Channel::recvMessage(Client *sender, const std::string &msg) const
{
	std::string str(":");
	str += sender->getNick() + " PRIVMSG " + Name + " " + msg;
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
		if (_client->second == INVITED)
			continue;
		SEND("s", msg.c_str());
	}
}

void Channel::revokeInvite()
{
	for (auto it = clientMap.begin(); it != clientMap.end(); it++)
	{
		if (it->second == INVITED)
			clientMap.erase(it->first);
	}
}

void Channel::clientSetState(Client *client, ClientState state)
{
	clientMap[client] = state;
}

const std::string Channel::getModeStr() const
{
	std::string str("");
	std::string params("");
	if (channelModes == 0)
		return str;
	str += "+";
	if (channelModes & INVITE_ONLY)
		str += "i";
	if (channelModes & TOPIC_RESTRICTION)
		str += "t";
	if (channelModes & CHANNEL_KEY)
	{
		str += "k";
		params += Key;
	}
	if (channelModes & USER_LIMIT)
	{
		str += "l";
		if (params.size() != 0)
			params += " ";
		params += UserLimit;
	}
	if (params.size() != 0)
		str += " ";
	str += params;
	return str;
}

#undef CHANNEL
