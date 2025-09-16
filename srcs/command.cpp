#include <Message.hpp>
#include <Client.hpp>
#include <Server.hpp>
#include <iostream>

#define buildCmd(id)				\
	void cmd##id(CmdBody body) { std::cout << "Command "#id" got called\n"; (void)body; }

#define CASE(id) case id: cmd##id(body); break;

void cmdNICK(CmdBody &body)
{
	if (body.params.size() <= 1)
	{
		server.sendError(431, body.client.getFd());
		return ;
	}
	std::string nick = body.params[1];
	std::map<int, Client> &clients = server.getClientMap();
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second.getNick() == nick)
		{
			server.sendError(433, body.client.getFd());
			return;
		}
	}
	body.client.setNick(nick);
}

buildCmd(USER);
buildCmd(JOIN);
buildCmd(PART);
buildCmd(PRIVMSG);
buildCmd(QUIT);
buildCmd(KICK);
buildCmd(INVITE);
buildCmd(TOPIC);
buildCmd(MODE);
buildCmd(UNKNOWN);

void executeCommand(const IrcMessage &msg, Client &client)
{
	CmdBody body(client, msg);
	switch (msg.id)
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
		CASE(UNKNOWN)
	}
}
