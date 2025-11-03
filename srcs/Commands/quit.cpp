/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wirare <wirare@42angouleme.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 18:38:35 by wirare            #+#    #+#             */
/*   Updated: 2025/11/03 18:38:48 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <Commands.hpp>
#include <ErrorCode.hpp>
#include <Server.hpp>
#include <Client.hpp>
#include <Send.hpp>

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
