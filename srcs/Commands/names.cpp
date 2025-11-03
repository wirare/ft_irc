/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   names.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wirare <wirare@42angouleme.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 18:37:13 by wirare            #+#    #+#             */
/*   Updated: 2025/11/03 18:37:20 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <Commands.hpp>
#include <ErrorCode.hpp>
#include <Server.hpp>
#include <Client.hpp>
#include <Send.hpp>

CMD_DEF(NAMES)
{
	if (body.params.size() == 1)
		SEND_ERR(ERR_NEEDMOREPARAMS, _NICK, body.params[0]);
	if (body.client->getState() != AUTH)
		return ;

	std::vector<std::string> channels = StringHelper::split(body.params[1], ',');
	const std::string &_nick = body.client->getNick();
	const char *nick = _nick.c_str();

	for (size_t i = 0; i != channels.size(); i++)
	{
		Channel *chan = server.getChannel(channels[i]);
		if (!chan)
			continue;
		auto clientMap = chan->getClientMap();
		std::string msg(nick);
		msg += " = " + chan->getName() + " :";
		for (auto it = clientMap.begin(); it != clientMap.end(); it++)
			msg += " " + it->first->getNick();
		SEND("odss", RPL_NAMEREPLY, " ", msg.c_str());
		SEND("odsssss", RPL_ENDOFNAMES, " ", nick, " ", channels[i].c_str(), " :End of /NAMES list");
	}
}
