/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ellanglo <ellanglo@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 16:44:48 by ellanglo          #+#    #+#             */
/*   Updated: 2025/09/30 20:31:46 by ellanglo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#pragma once
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
class Channel
{
	DECLARE(std::string, Topic);
	DECLARE(std::string, Name);
	DECLARE(std::string, Key);
	DECLARE(unsigned int, UserLimit);

	public:
		Channel(const std::string &name, const Client &client): Topic(""), Name(name)
		{
			clientMap[client] = OP;
		}
		~Channel();

		inline int getChannelModes() const { return channelModes; }
		inline bool hasMode(ChannelModeFlag mode) { return channelModes & mode; }
		inline void toggleMode(ChannelModeFlag mode) { channelModes ^= mode; }
		inline const std::map<Client, ClientState> getClientMap() const { return clientMap; }
		void addClient(Client &client, ClientState state, const std::string &key = "");

	private:
		std::map<Client, ClientState> clientMap;
		int channelModes;
};
#undef CHANNEL
