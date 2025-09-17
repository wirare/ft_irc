#include <Message.hpp>
#include <Client.hpp>
#include <Server.hpp>
#include <iostream>

#define CALL_LOG(id)\
	std::cout << "Command "#id" got called\n"

#define buildCmd(id)				\
	void cmd##id(CmdBody body) { CALL_LOG(id); (void)body; }

#define CASE(id) case id: cmd##id(body); break;

void cmdNICK(CmdBody &body)
{
	CALL_LOG(NICK);
	if (body.params.size() <= 1)
		SEND_ERR(431);
	std::string nick = body.params[1];
	std::map<int, Client> &clients = server.getClientMap();
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second.getNick() == nick)
			SEND_ERR(433);
	}
	body.client.setNick(nick);
}

void cmdPASS(CmdBody &body)
{
	CALL_LOG(PASS);
	if (body.client.getState() != WAIT_PASS)
		SEND_ERR(462);
	if (body.params.size() <= 1)
		SEND_ERR(461);
	if (body.params[1] != server.getPassword())
		SEND_ERR(464);
	body.client.setState(WAIT_NICK);
}

void cmdPING(CmdBody &body)
{
	CALL_LOG(PING);
	if (body.params.size() <= 1)
		SEND_ERR(461);
	std::string msg(":localhost PONG");
	msg += body.params[1];
	send(body.client.getFd(), msg.c_str(), msg.size(), 0);
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
		CASE(PASS)
		CASE(PING)
		CASE(UNKNOWN)
	}
}
