/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ATarget.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wirare <wirare@42angouleme.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 22:38:15 by wirare            #+#    #+#             */
/*   Updated: 2025/10/04 15:23:22 by ellanglo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#pragma once

#include <string>

class Client;

class ATarget
{
	public:
		virtual void recvMessage(const Client *client, const std::string &msg) const = 0;
		virtual ~ATarget() {};
};
