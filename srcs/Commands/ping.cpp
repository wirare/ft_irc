/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wirare <wirare@42angouleme.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 18:36:19 by wirare            #+#    #+#             */
/*   Updated: 2025/11/03 18:41:20 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <Commands.hpp>
#include <ErrorCode.hpp>
#include <Server.hpp>
#include <Client.hpp>
#include <Send.hpp>

CMD_DEF(PING)
{
	if (body.params.size() <= 1)
		SEND_ERR(ERR_NEEDMOREPARAMS, _NICK, body.params[0]);
	SEND("ss", "PONG", body.params[1].c_str());
}
