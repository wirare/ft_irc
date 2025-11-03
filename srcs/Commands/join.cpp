/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wirare <wirare@42angouleme.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 18:36:52 by wirare            #+#    #+#             */
/*   Updated: 2025/11/03 18:37:10 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <Commands.hpp>
#include <ErrorCode.hpp>
#include <Server.hpp>
#include <Client.hpp>
#include <Send.hpp>

CMD_DEF(JOIN)
{
	if (body.params.size() == 1)
		SEND_ERR(ERR_NEEDMOREPARAMS, _NICK, body.params[0]);
	if (body.client->getState() != AUTH)
		return ;
	if (body.params[1] == "0")
	{
		std::vector<Channel*> channels = server.getClientChannel(body.client);
		for (auto it = channels.begin(); it != channels.end(); it++)
			executeCommandInternal(PART, StringHelper::makeVector("PART ", (*it)->getName()), body.client);
		return;
	}
	std::vector<std::string> channels = StringHelper::split(body.params[1], ',');
	std::vector<std::string> keys;
	bool hasKey = body.params.size() >= 3;
	if (hasKey)
		keys = StringHelper::split(body.params[2], ',');
	for (size_t i = 0; i != channels.size(); i++)
	{
		if (!StringHelper::checkChannelFormat(channels[i]))
			SEND_ERR_CONTINUE(ERR_BADCHANMASK, _NICK, channels[i]);
		Channel *chan = server.getChannel(channels[i]);
		if (chan)
		{
			if (hasKey && i <= keys.size() - 1)
				chan->addClient(body.client, NORMAL, keys[i]);
			else
				chan->addClient(body.client, NORMAL);
		}
		else
		{
			chan = server.createChannel(channels[i]);
			chan->addClient(body.client, OP);
		}
	}
}
