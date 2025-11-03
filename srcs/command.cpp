/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wirare <wirare@42angouleme.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 15:29:22 by wirare            #+#    #+#             */
/*   Updated: 2025/11/03 18:41:15 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <Commands.hpp>

buildCmd(UNKNOWN);

void executeCommandInternal(CommandId id, std::vector<std::string> msg, Client *client)
{
	CmdBody body(client, msg);
	switch (id)
	{
		CASE(NICK)
		CASE(USER)
		CASE(JOIN)
		CASE(PART)
		CASE(PRIVMSG)
		CASE(QUIT)
		CASE(KICK)
		CASE(INVITE)
		CASE(TOPIC)
		CASE(MODE)
		CASE(PASS)
		CASE(PING)
		CASE(NAMES)
		CASE(UNKNOWN)
	}
}

void executeCommand(const IrcMessage &msg, Client *client)
{
	executeCommandInternal(msg.id, msg.params, client);
}
