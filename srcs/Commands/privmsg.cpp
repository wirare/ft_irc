/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wirare <wirare@42angouleme.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 18:37:35 by wirare            #+#    #+#             */
/*   Updated: 2025/11/03 18:37:44 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <Commands.hpp>
#include <ErrorCode.hpp>
#include <Server.hpp>
#include <Client.hpp>
#include <Send.hpp>

CMD_DEF(PRIVMSG)
{
	if (body.params.size() != 3)
		return;

	std::vector<std::string> targets = StringHelper::split(body.params[1], ',');
	std::map<int, Client*> &clientMap = server.getClientMap();

	for (auto _target = targets.begin(); _target != targets.end(); _target++)
	{
		ATarget *target;
		bool set = false;
		for (auto _client = clientMap.begin(); _client != clientMap.end(); _client++)
		{
			if (_target->data() == _client->second->getNick())
			{
				target = _client->second;
				set = true;
			}
		}
		if (!set)
		{
			Channel *chan = server.getChannel(_target->data());
			if (chan)
			{
				target = chan;
				set = true;
			}
		}
		if (set)
			target->recvMessage(body.client, body.params[2]);
	}
}
