/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ellanglo <ellanglo@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 16:44:48 by ellanglo          #+#    #+#             */
/*   Updated: 2025/09/28 20:25:34 by ellanglo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#pragma once
#include <map>
#include <Client.hpp>
#include <DeclMacro.hpp>
#include <string>

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

class Channel
{
	DECLARE(std::string, Topic);
	DECLARE(std::string, Name);

	public:
		Channel(const std::string &name, const Client &client): Topic(""), Name(name)
		{
			opMap[client] = OP;
		}
		~Channel();

		inline int getChannelModes() const { return channelModes; };
		inline bool hasMode(ChannelModeFlag mode) { return channelModes & mode; }
		inline void toggleMode(ChannelModeFlag mode) { channelModes ^= mode; }

	private:
		std::map<Client, ClientState> opMap;
		int channelModes;
};
