#include <ATarget.hpp>
#include <Channel.hpp>
#include <Message.hpp>
#include <Client.hpp>
#include <Server.hpp>
#include <iostream>
#include <Send.hpp>
#include <StringHelper.hpp>
#include <auto.hpp>
#include <Send.hpp>
#include <RPL_list.hpp>
#include <ctime>
#include <ErrorCode.hpp>

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
		SEND_ERR(ERR_PASSWDMISMATCH, _NICK);
	body.client->setState(POST_PASS);
	if (body.params.size() <= 1)
		SEND_ERR(ERR_NONICKNAMEGIVEN, _NICK);
	std::string nick = body.params[1];
	std::map<int, Client*> clients = server.getClientMap();
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

CMD_DEF(PASS)
{
	if (body.client->getState() != NEW)
		SEND_ERR(ERR_ALREADYREGISTERED, _NICK);
	if (body.params.size() <= 1)
		SEND_ERR(ERR_NEEDMOREPARAMS, body.params[0]);
	body.client->setLastPass(body.params[1]);
}

CMD_DEF(PING)
{
	if (body.params.size() <= 1)
		SEND_ERR(ERR_NEEDMOREPARAMS, body.params[0]);
	SEND("ss", "PONG", body.params[1].c_str());
}

CMD_DEF(USER)
{
	if (body.params.size() <= 4)
		SEND_ERR(ERR_NEEDMOREPARAMS, body.params[0]);
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

CMD_DEF(JOIN)
{
	if (body.params.size() == 1)
		SEND_ERR(ERR_NEEDMOREPARAMS, body.params[0]);
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
			SEND_ERR_CONTINUE(ERR_BADCHANMASK, _NICK, channels[i]);
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
		}
	}
}

CMD_DEF(NAMES)
{
	if (body.params.size() == 1)
		SEND_ERR(ERR_NEEDMOREPARAMS, body.params[0]);
	if (body.client->getState() != AUTH)
		return ;

	std::vector<std::string> channels = StringHelper::split(body.params[1], ',');
	const std::string &_username = body.client->getUsername();
	const char *username = _username.c_str();

	for (size_t i = 0; i != channels.size(); i++)
	{
		Channel *chan = server.getChannel(channels[i]);
		if (!chan)
			continue;
		auto clientMap = chan->getClientMap();
		std::string msg(username);
		msg += " = " + chan->getName() + " :";
		for (auto it = clientMap.begin(); it != clientMap.end(); it++)
			msg += " " + it->first->getNick();
		SEND("odss", RPL_NAMEREPLY, " ", msg.c_str());
		SEND("odsssss", RPL_ENDOFNAMES, " ", username, " ", channels[i].c_str(), " :End of /NAMES list");
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
			target->recvMessage(body.client, body.params[2]);
	}
}

CMD_DEF(TOPIC)
{
	if (body.params.size() == 1)
		SEND_ERR(ERR_NEEDMOREPARAMS, body.params[0]);

	Channel *chan = server.getChannel(body.params[1]);
	if (!chan)
		SEND_ERR(ERR_NOSUCHCHANNEL, body.params[1]);

	if (!chan->hasClient(body.client))
		SEND_ERR(ERR_NOTONCHANNEL, _NICK, body.params[1]);

	if (body.params.size() == 2)
	{
		std::string topic = chan->getTopic();
		time_t _time = chan->getTopicTime();
		if (topic == "")
			SEND("odsssss", RPL_NOTOPIC, " ", body.client->getUsername().c_str(), " ", chan->getName().c_str(), " :No topic is set");
		else
		{
			SEND("odssssss", RPL_TOPIC, " ", body.client->getUsername().c_str(), " ", chan->getName().c_str(), " :", topic.c_str());
			SEND("odsssssssl", RPL_TOPICWHOTIME, " ", body.client->getUsername().c_str(), " ", chan->getName().c_str(), " ", chan->getTopicChanger().c_str(), " ", _time);
		}
	}

	if (body.params.size() == 3)
	{
		if (chan->hasMode(TOPIC_RESTRICTION) && chan->getClientMap().at(body.client) != OP)
			SEND_ERR(ERR_CHANOPRIVSNEEDED, _NICK, body.params[1]);
		std::string topic = body.params[2];
		topic.erase(0,1);
		chan->setTopic(topic);
		chan->setTopicTime(std::time(NULL));
		chan->setTopicChanger(body.client->getNick());
		std::string msg(":");
		msg += body.client->getNick() + "!";
		msg += body.client->getUsername() + "@";
		msg += body.client->getHostname() + " TOPIC :" + topic;
		chan->broadcast(msg);
	}
}

CMD_DEF(PART)
{
	if (body.params.size() == 1)
		SEND_ERR(ERR_NEEDMOREPARAMS, body.params[0]);

	std::vector<std::string> channels = StringHelper::split(body.params[1], ',');
	std::string reason(" :");
	if (body.params.size() >= 3)
		reason += body.params[2];
	for (size_t i = 0; i != channels.size(); i++)
	{
		Channel *chan = server.getChannel(channels[i]);
		if (!chan)
			SEND_ERR_CONTINUE(ERR_NOSUCHCHANNEL, channels[i]);
		if (!chan->hasClient(body.client))
			SEND_ERR_CONTINUE(ERR_NOTONCHANNEL, _NICK, channels[i]);

		chan->broadcast(server.buildMessage("cssssssss", _NICK.c_str(), "!", body.client->getUsername().c_str(), "@", body.client->getHostname().c_str(), " PART ", channels[i].c_str(), reason.c_str()));
		chan->delClient(body.client);
	}
}

CMD_DEF(QUIT)
{
	std::vector<Channel*> channels = server.getClientChannel(body.client);
	std::string reason(" :");
	if (body.params.size() >= 2)
		reason += body.params[1];
	for (size_t i = 0; i != channels.size(); i++)
	{
		channels[i]->broadcast(server.buildMessage("cssssssss", _NICK.c_str(), "!", body.client->getUsername().c_str(), "@", body.client->getHostname().c_str(), " QUIT ", channels[i]->getName().c_str(), reason.c_str()));
		channels[i]->delClient(body.client);
	}
	SEND("ssssssss", "ERROR :Closing Link: ", _NICK.c_str(), "[", body.client->getHostname().c_str(), "] (Quit: ", body.params[1].c_str(), ")");
	server.deleteClient(body.client);
}

buildCmd(KICK);
buildCmd(INVITE);
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
