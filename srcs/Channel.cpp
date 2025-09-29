/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wirare <wirare@42angouleme.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 19:22:04 by wirare            #+#    #+#             */
/*   Updated: 2025/09/29 19:37:55 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <Channel.hpp>
#include <Server.hpp>
#define CHANNEL
#include <Send.hpp>

void Channel::addClient(Client &client, ClientState state, const std::string &key)
{
	if (hasMode(USER_LIMIT) && clientMap.size() >= UserLimit)
		SEND_ERR(471);
	if (hasMode(CHANNEL_KEY) && key != Key)
		SEND_ERR(475);
	if (hasMode(INVITE_ONLY))
	{
		auto clientState = clientMap.find(client);
		if (clientState == clientMap.end() || clientState->second != INVITED)
			SEND_ERR(473);
	}
	clientMap[client] = state;
}
#undef CHANNEL
