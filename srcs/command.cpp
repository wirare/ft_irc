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
	if (body.client.getState() != WAIT_NICK)
		SEND_ERR(462);
	std::string nick = body.params[1];
	std::map<int, Client> &clients = server.getClientMap();
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second.getNick() == nick)
			SEND_ERR(433);
	}
	body.client.setNick(nick);
	body.client.setState(WAIT_USER);
}

// probleme avec le body.params qui arrive dans ma fonction
void cmdUSER(CmdBody &body)
{
	CALL_LOG(USER);
	if (body.params.size() < 5)
		SEND_ERR(461);
	if (body.client.getState() != WAIT_USER)
		SEND_ERR(462);
	if (body.client.getState() == AUTH)
		SEND_ERR(462);

	body.client.setUsername(body.params[1]);

	std::string param = "Param = " + body.params[4] + '\n';
    send(body.client.getFd(), param.c_str(), param.size(), 0);

	std::string realName;
	for (size_t i = 4; i < body.params.size(); i++) {
		if (i != body.params.size())
			realName += " ";
		realName += body.params[i];
	}
	if (!realName.empty() && realName[0] == ':')
		realName.erase(0,1);

	body.client.setRealname(realName);
	body.client.setState(AUTH);

	std::string realname = "RealName = " + body.client.getRealname() + "|\n";
	std::string username = "UserName = " + body.client.getUsername() + '\n';
	std::string nick = "Nick = " + body.client.getNick() + '\n';

	std::string welcome = ":localhost 001 " + body.client.getNick() + " :Welcome to our IRC\r\n";
    send(body.client.getFd(), welcome.c_str(), welcome.size(), 0);
	send(body.client.getFd(), realname.c_str(), realname.size(), 0);
	send(body.client.getFd(), username.c_str(), username.size(), 0);
	send(body.client.getFd(), nick.c_str(), nick.size(), 0);
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
