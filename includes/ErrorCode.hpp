/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorCode.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wirare <wirare@42angouleme.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 18:17:41 by wirare            #+#    #+#             */
/*   Updated: 2025/10/09 19:43:17 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#pragma once
#include <UsefullMacro.hpp>
#include <string>

#define ERR(x) case x: return ERR##x

#define ERR401 "No such nick/channel"
#define ERR403 "No such channel"
#define ERR433 "Nickname is already in use"
#define ERR431 "No nickname given"
#define ERR442 "You're not on that channel"
#define ERR461 "Not enough parameters"
#define ERR462 "You may not reregister"
#define ERR464 "Password incorrect"
#define ERR471 "Cannot join channel (+l)"
#define ERR473 "Cannot join channel (+i)"
#define ERR475 "Cannot join channel (+k)"
#define ERR476 "Bad Channel Mask"
#define ERR482 "You're not channel operator"

#define ERR_NOSUCHNICK 401
#define ERR_NOSUCHCHANNEL 403
#define ERR_NICKNAMEINUSE 433
#define ERR_NONICKNAMEGIVEN 431
#define ERR_NOTONCHANNEL 442
#define ERR_NEEDMOREPARAMS 461
#define ERR_ALREADYREGISTERED 462
#define ERR_PASSWDMISMATCH 464
#define ERR_CHANNELISFULL 471
#define ERR_INVITEONLYCHAN 473
#define ERR_BADCHANNELKEY 475
#define ERR_BADCHANMASK 476
#define ERR_CHANOPRIVSNEEDED 482

inline std::string getErrMsg(int err)
{
	switch (err)
	{
		ERR(401);
		ERR(403);
		ERR(433);
		ERR(431);
		ERR(442);
		ERR(461);
		ERR(462);
		ERR(464);
		ERR(471);
		ERR(473);
		ERR(475);
		ERR(476);
		ERR(482);
		default: return "Unknown Error";
	}
}


