#pragma once

#include "auto.hpp"
#include <cstring>
#include <string>
#include <vector>
#include <Client.hpp>
#include <iostream>
#include <StringHelper.hpp>

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
	PING,
	NAMES
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
			else cmp(NAMES);
			else id = UNKNOWN;
		}
		
		std::string prefix;
		CommandId id;
		std::vector<std::string> params;

	private:
		std::vector<std::string> IrcSplit(const std::string& str)
		{
			std::vector<std::string> tokens = StringHelper::split(str, ' ');
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
			std::vector<std::string> return_tokens;
			for (auto it = tokens.begin(); it != tokens.end(); it++)
			{
				if (!it->empty())
					return_tokens.push_back(*it);
			}
			return return_tokens;
		}
};
