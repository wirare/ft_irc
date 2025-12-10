/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wirare <wirare@42angouleme.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 18:25:43 by wirare            #+#    #+#             */
/*   Updated: 2025/12/10 20:56:32 by ellanglo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#pragma once
#include <Client.hpp>
#include <vector>
#include <Message.hpp>
#include <string>

#define CALL_LOG(id)\
	std::cout << "Command "#id" got called\n"

#define buildCmd(id)				\
	void cmd##id(CmdBody body) { (void)body; }

#define CALL(id) cmd##id(body)
#define CASE(id) case id: CALL(id); break;

#define CMD_DEF(id) \
	void cmd##id(CmdBody &body)

struct CmdBody
{
	CmdBody(Client *client, const IrcMessage& msg): client(client), params(msg.params) {};
	CmdBody(Client *client, const std::vector<std::string> &params): client(client), params(params) {};
	Client *client;
	std::vector<std::string> params;
};

inline std::ostream &operator<<(std::ostream &os, CmdBody &body)
{
	os << "CmdBody client informations: \n{\n" << *body.client;
	os << "}\nCmdBody params: {";
	for (std::vector<std::string>::iterator it = body.params.begin(); it != body.params.end(); ++it)
	{
		os << it->data();
		if (it + 1 != body.params.end())
			os << ", ";
	}
	os << "}\n";
	return os;
}

void executeCommand(const IrcMessage &msg, Client *client);
void executeCommandInternal(CommandId id, std::vector<std::string> msg, Client *client);

CMD_DEF(NICK);
CMD_DEF(PASS);
CMD_DEF(PING);
CMD_DEF(USER);
CMD_DEF(JOIN);
CMD_DEF(NAMES);
CMD_DEF(PRIVMSG);
CMD_DEF(TOPIC);
CMD_DEF(PART);
CMD_DEF(QUIT);
CMD_DEF(KICK);
CMD_DEF(INVITE);
CMD_DEF(MODE);
