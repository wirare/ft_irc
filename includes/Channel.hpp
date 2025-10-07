/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ellanglo <ellanglo@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 16:44:48 by ellanglo          #+#    #+#             */
/*   Updated: 2025/10/07 17:28:59 by ellanglo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#pragma once
#include <ATarget.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <Client.hpp>
#include <DeclMacro.hpp>
#include <string>
#include <auto.hpp>

typedef enum
{
	INVITE_ONLY = 1 << 0,
	TOPIC_RESTRICTION = 1 << 1,
	CHANNEL_KEY = 1 << 2,
	USER_LIMIT = 1 << 3,
	OP_PRIVILEGE = 1 << 4,
}	ChannelModeFlag;

typedef enum
{
	INVITED,
	OP,
	NORMAL,
}	ClientState;

#define CHANNEL
class Channel: public ATarget
{
	DECLARE(int, Id);
	DECLARE(std::string, Topic);
	DECLARE(std::string, Name);
	DECLARE(std::string, Key);
	DECLARE(unsigned int, UserLimit);

	public:
		Channel() {};
		Channel(const std::string &name);
		~Channel() {};

		inline int getChannelModes() const { return channelModes; }
		inline bool hasMode(ChannelModeFlag mode) { return channelModes & mode; }
		inline void toggleMode(ChannelModeFlag mode) { channelModes ^= mode; }
		inline const std::map<Client*, ClientState> getClientMap() const { return clientMap; }
		void addClient(Client *client, ClientState state, const std::string &key = "");
		void successfulJoin(Client *client);
		inline bool hasClient(const Client *client) const
		{
			for (auto it = clientMap.begin(); it != clientMap.end(); it++)
			{
				if (it->first == client)
					return true;
			}
			return false;
		}
		inline bool operator==(const Channel &other) { return Id == other.Id; } 
		void recvMessage(Client *client, const std::string &msg) const;
		void broadcast(const std::string &msg, Client *sender) const;
		void broadcast(const std::string &msg, const std::vector<Client *> &exceptions = std::vector<Client *>()) const;

	private:
		std::map<Client*, ClientState> clientMap;
		int channelModes;    
		Channel(const Channel&);
		Channel& operator=(const Channel&);
};
#undef CHANNEL
