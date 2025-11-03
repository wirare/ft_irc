/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wirare <wirare@42angouleme.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 18:36:36 by wirare            #+#    #+#             */
/*   Updated: 2025/11/03 18:36:44 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <Commands.hpp>
#include <ErrorCode.hpp>
#include <Server.hpp>
#include <Client.hpp>
#include <Send.hpp>

CMD_DEF(USER)
{
	if (body.params.size() <= 4)
		SEND_ERR(ERR_NEEDMOREPARAMS, _NICK, body.params[0]);
	if (body.client->getState() != POST_PASS)
		SEND_ERR(ERR_ALREADYREGISTERED, _NICK);
	
	body.client->setUsername(body.params[1]);

	std::string realName;
	for (size_t i = 4; i < body.params.size(); i++) 
	{
		if (i != body.params.size())
			realName += " ";
		realName += body.params[i];
	}
	if (!realName.empty() && realName[0] == ':')
		realName.erase(0,1);

	body.client->setRealname(realName);
	body.client->setState(AUTH);
	server.sendSuccessfulRegister(body.client->getFd());
}
