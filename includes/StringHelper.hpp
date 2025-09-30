/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringHelper.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ellanglo <ellanglo@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 19:39:02 by ellanglo          #+#    #+#             */
/*   Updated: 2025/09/30 18:03:52 by ellanglo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <auto.hpp>

namespace StringHelper
{
	std::vector<std::string> split(const std::string& str, char c)
	{
		std::vector<std::string> tokens;
		std::string token;
		std::istringstream tokenStream(str);
		while (std::getline(tokenStream, token, c))
			tokens.push_back(token);
		return tokens;
	}

	bool checkChannelFormat(const std::string& str)
	{
		if (str[0] != '&')
			return false;
		for (auto it = str.begin()++; it != str.end(); it++)
		{
			char c = *it.base();
			if (c == ' ' || c == 0x07 || c == ',')
				return false;
		}
		return true;
	}
};
