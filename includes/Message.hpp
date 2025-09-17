#pragma once

#include <cstring>
#include <sstream>
#include <string>
#include <vector>
#include <Client.hpp>
#include <iostream>

typedef enum {
	UNKNOWN,
	NICK,
	USER,
	JOIN,
	PART,
	PRIVMSG,
	QUIT,
	KICK,
	INVITE,
	TOPIC,
	MODE,
	PASS,
	PING
}	CommandId;

#define cmp(Id) if (params[0] == #Id) id = Id

class IrcMessage
{
	public:
		IrcMessage(std::string message)
		{
			params = IrcSplit(message);
			cmp(NICK);
			else cmp(USER);
			else cmp(JOIN);
			else cmp(PART);
			else cmp(PRIVMSG);
			else cmp(QUIT);
			else cmp(KICK);
			else cmp(INVITE);
			else cmp(TOPIC);
			else cmp(MODE);
			else cmp(PASS);
			else cmp(PING);
			else id = UNKNOWN;
		}
		
		std::string prefix;
		CommandId id;
		std::vector<std::string> params;

	private:
		std::vector<std::string> IrcSplit(const std::string& str)
		{
			std::vector<std::string> tokens;
			std::string token;
			std::istringstream tokenStream(str);
			while (std::getline(tokenStream, token, ' '))
				tokens.push_back(token);
			int concat = 0;
			for (int i = 0; i != (int)tokens.size(); i++)
			{
				if (concat)
					tokens[concat] += tokens[i];
				else if (tokens[i][0] == ':')
					concat = i;
			}
			if (concat)
				tokens.resize(concat+1);
			std::string &last = tokens.back();
			if (last.at(last.size()-1) == '\r')
				last.resize(last.size()-1);
			return tokens;
		}

};

struct CmdBody
{
	CmdBody(Client &client, const IrcMessage& msg): client(client), params(msg.params) {};
	Client &client;
	std::vector<std::string> params;
};

inline std::ostream &operator<<(std::ostream &os, CmdBody &body)
{
	os << "CmdBody client informations: \n{\n" << body.client;
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

void executeCommand(const IrcMessage &msg, Client &client);
