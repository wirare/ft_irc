/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wirare <wirare@42angouleme.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 18:35:57 by wirare            #+#    #+#             */
/*   Updated: 2025/11/03 18:36:07 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <Commands.hpp>
#include <ErrorCode.hpp>
#include <Server.hpp>
#include <Client.hpp>
#include <Send.hpp>

CMD_DEF(PASS)
{
	if (body.client->getState() != NEW)
		SEND_ERR(ERR_ALREADYREGISTERED, _NICK);
	if (body.params.size() <= 1)
		SEND_ERR(ERR_NEEDMOREPARAMS, body.params[0]);
	body.client->setLastPass(body.params[1]);
}
