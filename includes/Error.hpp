/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ellanglo <ellanglo@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 18:57:59 by ellanglo          #+#    #+#             */
/*   Updated: 2025/09/11 19:05:11 by ellanglo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <stdexcept>
#include <string>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define LOCATION(file, func, line) \
    std::string("In file: ") + file + \
    "\nIn function: '" + func + "'" + \
    "\nAt line: " + STR(line)

#define EXCEPTION(except, msg) \
	except(std::string(msg) + "\n" + LOCATION(__FILE__, __func__, __LINE__))

#define CANT_OPEN_SOCKET EXCEPTION(std::runtime_error, "Can't open socket")
