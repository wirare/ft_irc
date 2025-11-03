/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wirare <wirare@42angouleme.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 18:39:24 by wirare            #+#    #+#             */
/*   Updated: 2025/11/03 18:39:31 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <Commands.hpp>
#include <ErrorCode.hpp>
#include <Server.hpp>
#include <Client.hpp>
#include <Send.hpp>

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
