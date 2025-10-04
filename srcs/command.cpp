#include "ATarget.hpp"
#include <Channel.hpp>
#include <Message.hpp>
#include <Client.hpp>
#include <Server.hpp>
#include <iostream>
#include <Send.hpp>
#include <StringHelper.hpp>
#include <auto.hpp>
#include <Send.hpp>

#define CALL_LOG(id)\
	std::cout << "Command "#id" got called\n"

#define buildCmd(id)				\
	void cmd##id(CmdBody body) { (void)body; }

#define CALL(id) cmd##id(body)
#define CASE(id) case id: CALL(id); CALL_LOG(id); break;

#define CMD_DEF(id) \
	void cmd##id(CmdBody &body)

CMD_DEF(NICK)
{
	if (body.client->getLastPass() != server.getPassword())
		SEND_ERR(464);
	body.client->setState(POST_PASS);
	if (body.params.size() <= 1)
		SEND_ERR(431);
	std::string nick = body.params[1];
	std::map<int, Client*> clients = server.getClientMap();
	for (auto it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second->getNick() == nick)
			SEND_ERR(433);
	}
	body.client->setNick(nick);
}

CMD_DEF(PASS)
{
	if (body.client->getState() != NEW)
		SEND_ERR(462);
	if (body.params.size() <= 1)
		SEND_ERR(461);
	body.client->setLastPass(body.params[1]);
}

CMD_DEF(PING)
{
	if (body.params.size() <= 1)
		SEND_ERR(461);
	SEND("ss", "PONG", body.params[1].c_str());
}

CMD_DEF(USER)
{
	if (body.params.size() <= 4)
		SEND_ERR(461);
	if (body.client->getState() != POST_PASS)
		SEND_ERR(462);
	
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

CMD_DEF(JOIN)
{
	if (body.params.size() == 1)
		SEND_ERR(461);
	if (body.client->getState() != AUTH)
		return ;

	std::vector<std::string> channels = StringHelper::split(body.params[1], ',');
	std::vector<std::string> keys;
	bool hasKey = body.params.size() >= 3;
	if (hasKey)
		keys = StringHelper::split(body.params[2], ',');
	for (size_t i = 0; i != channels.size(); i++)
	{
		if (!StringHelper::checkChannelFormat(channels[i]))
			SEND_ERR(476, CONTINUE);
		Channel *chan = server.getChannel(channels[i]);
		if (chan)
		{
			if (hasKey && i <= keys.size() - 1)
				chan->addClient(body.client, NORMAL, keys[i]);
			else
				chan->addClient(body.client, NORMAL);
		}
		else
		{
			Channel *newChan = server.createChannel(channels[i]);
			newChan->addClient(body.client, OP);
			newChan->successfulJoin(body.client);
		}
	}
}

CMD_DEF(NAMES)
{
	if (body.params.size() == 1)
		SEND_ERR(461);
	if (body.client->getState() != AUTH)
		return ;

	std::vector<std::string> channels = StringHelper::split(body.params[1], ',');
	const std::string &_username = body.client->getUsername();
	const char *username = _username.c_str();

	for (size_t i = 0; i != channels.size(); i++)
	{
		Channel *chan = server.getChannel(channels[i]);
		if (chan && StringHelper::checkChannelFormat(channels[i]))
		{
			auto clientMap = chan->getClientMap();
			for (auto it = clientMap.begin(); it != clientMap.end(); it++)
				SEND("csssss", username, " = ", chan->getName().c_str(), " :", it->first->getNick().c_str());
		}
		SEND("cssss", username, " ", channels[i].c_str(), " :End of /NAMES list");
	}
}

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
		{
			target->recvMessage(body.client, body.params[2]);
		}
	}
}

buildCmd(PART);
buildCmd(QUIT);
buildCmd(KICK);
buildCmd(INVITE);
buildCmd(TOPIC);
buildCmd(MODE);
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

