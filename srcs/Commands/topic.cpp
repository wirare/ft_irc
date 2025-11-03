/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wirare <wirare@42angouleme.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 18:37:55 by wirare            #+#    #+#             */
/*   Updated: 2025/11/03 18:38:05 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <Commands.hpp>
#include <ErrorCode.hpp>
#include <Server.hpp>
#include <Client.hpp>
#include <Send.hpp>

CMD_DEF(TOPIC)
{
	if (body.params.size() == 1)
		SEND_ERR(ERR_NEEDMOREPARAMS, _NICK, body.params[0]);

	Channel *chan = server.getChannel(body.params[1]);
	if (!chan)
		SEND_ERR(ERR_NOSUCHCHANNEL, body.params[1]);

	if (!chan->hasClient(body.client))
		SEND_ERR(ERR_NOTONCHANNEL, _NICK, body.params[1]);

	if (body.params.size() == 2)
	{
		std::string topic = chan->getTopic();
		time_t _time = chan->getTopicTime();
		if (topic == "")
			SEND("odsssss", RPL_NOTOPIC, " ", body.client->getUsername().c_str(), " ", chan->getName().c_str(), " :No topic is set");
		else
		{
			SEND("odssssss", RPL_TOPIC, " ", body.client->getUsername().c_str(), " ", chan->getName().c_str(), " :", topic.c_str());
			SEND("odsssssssl", RPL_TOPICWHOTIME, " ", body.client->getUsername().c_str(), " ", chan->getName().c_str(), " ", chan->getTopicChanger().c_str(), " ", _time);
		}
	}

	if (body.params.size() == 3)
	{
		if (chan->hasMode(TOPIC_RESTRICTION) && chan->getClientMap().at(body.client) != OP)
			SEND_ERR(ERR_CHANOPRIVSNEEDED, _NICK, body.params[1]);
		std::string topic = body.params[2];
		topic.erase(0,1);
		chan->setTopic(topic);
		chan->setTopicTime(std::time(NULL));
		chan->setTopicChanger(body.client->getNick());
		std::string msg = body.client->getFullName();
		msg += " TOPIC " + body.params[1] + " :" + topic;
		chan->broadcast(msg);
	}
}
