/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wirare <wirare@42angouleme.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 18:32:52 by wirare            #+#    #+#             */
/*   Updated: 2025/11/03 18:35:40 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <Commands.hpp>
#include <ErrorCode.hpp>
#include <Server.hpp>
#include <Client.hpp>
#include <Send.hpp>

CMD_DEF(NICK)
{
	if (body.client->getLastPass() != server.getPassword())
		SEND_ERR(ERR_PASSWDMISMATCH, _NICK);
	body.client->setState(POST_PASS);
	if (body.params.size() <= 1)
		SEND_ERR(ERR_NONICKNAMEGIVEN, _NICK);
	std::string nick = body.params[1];
	std::map<int, Client*> &clients = server.getClientMap();
	for (auto it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second->getNick() == nick)
		{
			if (body.client->getNick() == "")
				SEND_ERR(ERR_NICKNAMEINUSE, std::string("*"), nick);
			SEND_ERR(ERR_NICKNAMEINUSE, _NICK, nick);
		}
	}
	body.client->setNick(nick);
}
