/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ATarget.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wirare <wirare@42angouleme.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 22:38:15 by wirare            #+#    #+#             */
/*   Updated: 2025/10/07 17:28:38 by ellanglo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#pragma once

#include <string>

class Client;

class ATarget
{
	public:
		virtual void recvMessage(Client *client, const std::string &msg) const = 0;
		virtual ~ATarget() {};
};
