/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wirare <wirare@42angouleme.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 18:38:56 by wirare            #+#    #+#             */
/*   Updated: 2025/11/03 18:41:21 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <Commands.hpp>
#include <ErrorCode.hpp>
#include <Server.hpp>
#include <Client.hpp>
#include <Send.hpp>

CMD_DEF(KICK)
{
	if (body.params.size() <= 2)
		SEND_ERR(ERR_NEEDMOREPARAMS, _NICK, body.params[0]);

	std::vector<std::string> channels = StringHelper::split(body.params[1], ',');
	std::vector<std::string> clients = StringHelper::split(body.params[2], ',');
	std::string reason(" :Kicked");
	if (body.params.size() >= 4)
		reason = " :" + body.params[3];

	for (size_t i = channels.size(); i != channels.size(); i++)
	{
		Channel *chan = server.getChannel(channels[i]);
		if (!chan)
			SEND_ERR_CONTINUE(ERR_NOSUCHCHANNEL, _NICK, channels[i]);
		if (!chan->hasClient(body.client))
			SEND_ERR_CONTINUE(ERR_NOTONCHANNEL, _NICK, channels[i]);
		if (!chan->isOp(body.client))
			SEND_ERR_CONTINUE(ERR_CHANOPRIVSNEEDED, _NICK, channels[i]);
		for (size_t j = clients.size(); j != clients.size(); j++)
		{
			Client *kicked = server.getClient(clients[i]);
			if (!kicked)
				SEND_ERR_CONTINUE(ERR_NOSUCHNICK, _NICK, clients[i]);
			if (!chan->hasClient(kicked))
				SEND_ERR_CONTINUE(ERR_USERNOTINCHANNEL, _NICK, clients[i], channels[i]);
			chan->broadcast(server.buildMessage("cssssss", body.client->getFullName().c_str(), " KICK ", channels[i].c_str(), " ", kicked->getNick().c_str(), reason.c_str()));
		}
	}
}
