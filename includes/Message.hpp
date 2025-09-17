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
	MODE
}	CommandId;

#define cmp(Id) if (std::strcmp(cmd, #Id)) id = Id

class IrcMessage
{
	public:
		IrcMessage(std::string message)
		{
			params = IrcSplit(message);
			const char *cmd = params[0].c_str();
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
				tokens.resize(concat);
			return tokens;
		}

};

struct CmdBody
{
	CmdBody(Client &client, const IrcMessage& msg): client(client), params(msg.params) {};
	Client &client;
	std::vector<std::string> params;
};

void executeCommand(const IrcMessage &msg, Client &client);
