/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wirare <wirare@42angouleme.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 18:38:17 by wirare            #+#    #+#             */
/*   Updated: 2025/11/03 18:38:26 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <Commands.hpp>
#include <ErrorCode.hpp>
#include <Server.hpp>
#include <Client.hpp>
#include <Send.hpp>

CMD_DEF(PART)
{
	if (body.params.size() == 1)
		SEND_ERR(ERR_NEEDMOREPARAMS, _NICK, body.params[0]);

	std::vector<std::string> channels = StringHelper::split(body.params[1], ',');
	std::string reason(" :");
	if (body.params.size() >= 3)
		reason += body.params[2];
	for (size_t i = 0; i != channels.size(); i++)
	{
		Channel *chan = server.getChannel(channels[i]);
		if (!chan)
			SEND_ERR_CONTINUE(ERR_NOSUCHCHANNEL, channels[i]);
		if (!chan->hasClient(body.client))
			SEND_ERR_CONTINUE(ERR_NOTONCHANNEL, _NICK, channels[i]);
		chan->broadcast(server.buildMessage("cssss", body.client->getFullName().c_str(), " PART ", channels[i].c_str(), reason.c_str()));
		chan->delClient(body.client);
	}
}
