/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ellanglo <ellanglo@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 18:57:59 by ellanglo          #+#    #+#             */
/*   Updated: 2025/09/16 02:27:10 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <ErrorCode.hpp>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define LOCATION(file, func, line) \
    std::string("In file: ") + file + \
    "\nIn function: '" + func + "'" + \
    "\nAt line: " + STR(line)

#define EXCEPTION(except, msg) \
	except(std::string(msg) + "\n" + LOCATION(__FILE__, __func__, __LINE__))

#define CANT_OPEN_SOCKET EXCEPTION(std::runtime_error, "Can't open socket")
#define CANT_BIND_TO_PORT EXCEPTION(std::runtime_error, "Can't bind to port")
#define CANT_LISTEN_SOCKET EXCEPTION(std::runtime_error, "Can't listen on socket")
#define EPOLL_CTL_ADD_FAILURE EXCEPTION(std::runtime_error, "Error on EPOLL_CTL_ADD")
#define CANT_ACCEPT_CONNECTION EXCEPTION(std::runtime_error, "Can't accept the connection")
#define EPOLL_WAIT_FAILURE EXCEPTION(std::runtime_error, "Error on epoll_wait")
