/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wirare <wirare@42angouleme.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 18:39:43 by wirare            #+#    #+#             */
/*   Updated: 2025/11/03 18:40:40 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../includes/Commands.hpp"
#include <ErrorCode.hpp>
#include <Server.hpp>
#include <Client.hpp>
#include <Send.hpp>
#include <queue>

CMD_DEF(MODE)
{
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
			modeQueue.push(it->data());

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
					if (userLimit <= 0)
					{
						modeQueue.pop();
						continue;
					}
					chan->setUserLimit(userLimit);
					pMode.push(std::make_pair('l', true));
					pParams.push(modeQueue.front());
					modeQueue.pop();
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
				SEND_ERR_CONTINUE(ERR_UNKNOWNMODE, _NICK, modes[i]);
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

