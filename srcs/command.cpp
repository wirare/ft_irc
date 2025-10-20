#include <ATarget.hpp>
#include <Channel.hpp>
#include <Message.hpp>
#include <Client.hpp>
#include <Server.hpp>
#include <cstdlib>
#include <iostream>
#include <Send.hpp>
#include <StringHelper.hpp>
#include <auto.hpp>
#include <Send.hpp>
#include <RPL_list.hpp>
#include <ctime>
#include <ErrorCode.hpp>
#include <queue>
#include <stack>
#include <utility>

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
		SEND_ERR(ERR_NEEDMOREPARAMS, _NICK, body.params[0]);
	SEND("ss", "PONG", body.params[1].c_str());
}

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

CMD_DEF(JOIN)
{
	if (body.params.size() == 1)
		SEND_ERR(ERR_NEEDMOREPARAMS, _NICK, body.params[0]);
	if (body.client->getState() != AUTH)
		return ;
	if (body.params[1] == "0")
	{
		std::vector<Channel*> channels = server.getClientChannel(body.client);
		for (auto it = channels.begin(); it != channels.end(); it++)
			executeCommandInternal(PART, StringHelper::makeVector("PART ", (*it)->getName()), body.client);
		return;
	}
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
			chan = server.createChannel(channels[i]);
			chan->addClient(body.client, OP);
		}
	}
}

CMD_DEF(NAMES)
{
	if (body.params.size() == 1)
		SEND_ERR(ERR_NEEDMOREPARAMS, _NICK, body.params[0]);
	if (body.client->getState() != AUTH)
		return ;

	std::vector<std::string> channels = StringHelper::split(body.params[1], ',');
	const std::string &_nick = body.client->getNick();
	const char *nick = _nick.c_str();

	for (size_t i = 0; i != channels.size(); i++)
	{
		Channel *chan = server.getChannel(channels[i]);
		if (!chan)
			continue;
		auto clientMap = chan->getClientMap();
		std::string msg(nick);
		msg += " = " + chan->getName() + " :";
		for (auto it = clientMap.begin(); it != clientMap.end(); it++)
			msg += " " + it->first->getNick();
		SEND("odss", RPL_NAMEREPLY, " ", msg.c_str());
		SEND("odsssss", RPL_ENDOFNAMES, " ", nick, " ", channels[i].c_str(), " :End of /NAMES list");
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
		SEND_ERR(ERR_NEEDMOREPARAMS, _NICK, body.params[0]);

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
		std::string msg = body.client->getFullName();
		msg += " TOPIC " + body.params[1] + " :" + topic;
		chan->broadcast(msg);
	}
}

CMD_DEF(PART)
{
	if (body.params.size() == 1)
		SEND_ERR(ERR_NEEDMOREPARAMS, _NICK, body.params[0]);

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
		chan->broadcast(server.buildMessage("cssss", body.client->getFullName().c_str(), " PART ", channels[i].c_str(), reason.c_str()));
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
		channels[i]->broadcast(server.buildMessage("cssss", body.client->getFullName().c_str(), " QUIT ", channels[i]->getName().c_str(), reason.c_str()));
		channels[i]->delClient(body.client);
	}
	SEND("ssssssss", "ERROR :Closing Link: ", _NICK.c_str(), "[", body.client->getHostname().c_str(), "] (Quit: ", body.params[1].c_str(), ")");
	server.deleteClient(body.client);
}

CMD_DEF(KICK)
{
	if (body.params.size() <= 2)
		SEND_ERR(ERR_NEEDMOREPARAMS, _NICK, body.params[0]);

	std::vector<std::string> channels = StringHelper::split(body.params[1], ',');
	std::vector<std::string> clients = StringHelper::split(body.params[2], ',');
	std::string reason(" :Kicked");
	if (body.params.size() >= 4)
		reason = " :" + body.params[3];

	for (size_t i = channels.size(); i != channels.size(); i++)
	{
		Channel *chan = server.getChannel(channels[i]);
		if (!chan)
			SEND_ERR_CONTINUE(ERR_NOSUCHCHANNEL, _NICK, channels[i]);
		if (!chan->hasClient(body.client))
			SEND_ERR_CONTINUE(ERR_NOTONCHANNEL, _NICK, channels[i]);
		if (!chan->isOp(body.client))
			SEND_ERR_CONTINUE(ERR_CHANOPRIVSNEEDED, _NICK, channels[i]);
		for (size_t j = clients.size(); j != clients.size(); j++)
		{
			Client *kicked = server.getClient(clients[i]);
			if (!kicked)
				SEND_ERR_CONTINUE(ERR_NOSUCHNICK, _NICK, clients[i]);
			if (!chan->hasClient(kicked))
				SEND_ERR_CONTINUE(ERR_USERNOTINCHANNEL, _NICK, clients[i], channels[i]);
			chan->broadcast(server.buildMessage("cssssss", body.client->getFullName().c_str(), " KICK ", channels[i].c_str(), " ", kicked->getNick().c_str(), reason.c_str()));
		}
	}
}

CMD_DEF(INVITE)
{
	if (body.params.size() <= 2)
		SEND_ERR(ERR_NEEDMOREPARAMS, _NICK, body.params[0]);

	Client *invitee = server.getClient(body.params[1]);
	Channel *chan = server.getChannel(body.params[2]);
	if (!invitee)
		SEND_ERR(ERR_NOSUCHNICK, _NICK, body.params[1]);
	if (!chan)
		SEND_ERR(ERR_NOSUCHCHANNEL, _NICK, body.params[2]);
	if (!chan->hasClient(body.client))
		SEND_ERR(ERR_NOTONCHANNEL, _NICK, body.params[2]);
	if (chan->hasClient(invitee))
		SEND_ERR(ERR_USERONCHANNEL, _NICK, body.params[1], body.params[2]);
	if (chan->hasMode(INVITE_ONLY) && !chan->isOp(body.client))
		SEND_ERR(ERR_CHANOPRIVSNEEDED, _NICK, body.params[2]);
	chan->clientSetState(invitee, INVITED);
	SEND("odsssss", RPL_INVITING, _NICK.c_str(), " ", body.params[1].c_str(), " ", body.params[2].c_str());
}

CMD_DEF(MODE)
{
	std::cout << body << "\n";
	if (body.params.size() == 1)
		SEND_ERR(ERR_NEEDMOREPARAMS, _NICK, body.params[0]);

	Channel *chan = server.getChannel(body.params[1]);
	if (!chan)
		SEND_ERR(ERR_NOSUCHCHANNEL, _NICK, body.params[1]);
	if (!chan->hasClient(body.client))
		SEND_ERR(ERR_NOTONCHANNEL, _NICK, body.params[1]);

	if (body.params.size() == 2)
	{
		SEND("odssssss", RPL_CHANNELMODEIS, " ", _NICK.c_str(), " ", body.params[1].c_str(), " ", chan->getModeStr().c_str());
		SEND("odsssssd", RPL_CREATIONTIME, " ", _NICK.c_str(), " ", body.params[1].c_str(), " ", chan->getCreationTime());
		return;
	}

	std::queue<std::string> modeQueue;
	if (body.params.size() >= 4)
		for (auto it = body.params.begin() + 3; it != body.params.end(); it++)
		{
			std::cout << "Appending to modeQueue: " << it->data() << "\n";
			modeQueue.push(it->data());
		}

	bool currentMode;
	const std::string &modes = body.params[2];
	if (!(modes[0] == '+' || modes[0] == '-'))
		SEND_ERR(ERR_NEEDMOREPARAMS, _NICK, body.params[0]);
	std::queue<std::pair<char, bool> > pMode; 
	std::queue<std::pair<char, bool> > mMode;
	std::queue<std::string> pParams;
	std::queue<std::string> mParams;
	for (size_t i = 0; i != modes.size(); i++)
	{
		std::cout << "Processing: " << modes[i] << "char\n";
		if (!modeQueue.empty())
			std::cout << "Front queue: " << modeQueue.front() << "\n";
		switch (modes[i])
		{

			case '+': currentMode = true; break;
			case '-': currentMode = false; break;
			case 'i': 
			{
				if (!chan->isOp(body.client))
					SEND_ERR_CONTINUE(ERR_CHANOPRIVSNEEDED, _NICK, body.params[1])
				chan->setMode(INVITE_ONLY, currentMode);
				chan->revokeInvite();
				if (currentMode)
					pMode.push(std::make_pair('i', false));
				else
					mMode.push(std::make_pair('i', false));
				break;
			}
			case 'k':
			{
				if (!chan->isOp(body.client))
					SEND_ERR_CONTINUE(ERR_CHANOPRIVSNEEDED, _NICK, body.params[1])
				if (currentMode == true)
				{
					if (modeQueue.size() == 0)
						SEND_ERR_CONTINUE(ERR_NEEDMOREPARAMS, _NICK, body.params[0]);
					chan->setKey(modeQueue.front());
					pMode.push(std::make_pair('k', true));
					pParams.push(modeQueue.front());
					modeQueue.pop();
				}
				else
					mMode.push(std::make_pair('k', false));
				chan->setMode(CHANNEL_KEY, currentMode);
				break;
			}
			case 'o':
			{
				if (!chan->isOp(body.client))
					SEND_ERR_CONTINUE(ERR_CHANOPRIVSNEEDED, _NICK, body.params[1])
				if (modeQueue.size() == 0)
					SEND_ERR_CONTINUE(ERR_NEEDMOREPARAMS, _NICK, body.params[0]);
				const std::string& nick = modeQueue.front();
				Client *target = server.getClient(nick);
				modeQueue.pop();
				if (!target)
					SEND_ERR_CONTINUE(ERR_NOSUCHNICK, _NICK, nick);
				if (!chan->hasClient(target))
					SEND_ERR_CONTINUE(ERR_USERNOTINCHANNEL, _NICK, nick, body.params[1]);
				if (currentMode == true)
				{
					chan->clientSetState(target, OP);
					pMode.push(std::make_pair('o', true));
					pParams.push(nick);
				}
				else
				{
					chan->clientSetState(target, NORMAL);
					mMode.push(std::make_pair('o', true));
					mParams.push(nick);
				}
				break;
			}
			case 'l':
			{
				if (!chan->isOp(body.client))
					SEND_ERR_CONTINUE(ERR_CHANOPRIVSNEEDED, _NICK, body.params[1])
				if (currentMode == true)
				{
					if (modeQueue.size() == 0)
						SEND_ERR_CONTINUE(ERR_NEEDMOREPARAMS, _NICK, body.params[0]);
					int userLimit = std::atoi(modeQueue.front().c_str());
					modeQueue.pop();
					if (userLimit <= 0)
						continue;
					chan->setUserLimit(userLimit);
					pMode.push(std::make_pair('l', true));
					pParams.push(StringHelper::itoa(userLimit));
				}
				else
					mMode.push(std::make_pair('l', false));
				chan->setMode(USER_LIMIT, currentMode);
				break;
			}
			case 't':
			{
				if (!chan->isOp(body.client))
					SEND_ERR_CONTINUE(ERR_CHANOPRIVSNEEDED, _NICK, body.params[1])
				chan->setMode(TOPIC_RESTRICTION, currentMode);
				if (currentMode == true)
					pMode.push(std::make_pair('t', false));
				else
					mMode.push(std::make_pair('t', false));
				break;
			}
			default:
			{
				SEND_ERR_CONTINUE(ERR_UNKNOWNMODE, _NICK, modes[i]);
			}
		}
	}
	std::string modeCharStr(" :");
	std::string modeParamStr(" ");
	if (!pMode.empty())
	{
		modeCharStr += "+";
		while (!pMode.empty())
		{
			modeCharStr += pMode.front().first;
			if (pMode.front().second)
			{
				modeParamStr += pParams.front() + " ";
				pParams.pop();
			}
			pMode.pop();
		}
	}
	if (!mMode.empty())
	{
		modeCharStr += "-";
		while (!mMode.empty())
		{
			modeCharStr += mMode.front().first;
			if (mMode.front().second)
			{
				modeParamStr += mParams.front() + " ";
				mParams.pop();
			}
			mMode.pop();
		}
	}
	chan->broadcast(server.buildMessage("csssss", body.client->getFullName().c_str(), " MODE ", body.params[1].c_str(), modeCharStr.c_str(), modeParamStr.c_str()));
}

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
