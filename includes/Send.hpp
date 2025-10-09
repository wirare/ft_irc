/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Send.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ellanglo <ellanglo@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 20:50:36 by ellanglo          #+#    #+#             */
/*   Updated: 2025/10/09 19:42:04 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <UsefullMacro.hpp>
#ifndef BUILDER_MACRO
	#define BUILDER_MACRO
	#define BUILD_ERR_1(param)			param_str += param;
	#define BUILD_ERR_2(param1, param2) param_str += param1 + " " + param2;
	#define BUILD_ERR(...)				CONCAT(BUILD_ERR_, COUNT(__VA_ARGS__))
#endif

#ifdef SEND
	#undef SEND
	#undef SEND_ERR
	#undef SERVER_OPT
	#undef FD_NAME
#endif

#if defined(SERVER)
	#define SERVER_OPT
	#define FD_NAME fd
#elif defined(CHANNEL)
	#define _NICK client->getNick()
	#define SERVER_OPT server.
	#define FD_NAME client->getFd()
#elif defined(CLIENT)
	#define SERVER_OPT server.
	#define FD_NAME fd
#else
	#define _NICK body.client->getNick()
	#define SERVER_OPT server.
	#define FD_NAME body.client->getFd()
#endif

#define SEND(...) SERVER_OPT sendMessage(FD_NAME, SERVER_OPT buildMessage(__VA_ARGS__))

#define SEND_ERR(err, ...)								\
	{													\
		std::string param_str;							\
		BUILD_ERR(__VA_OPT__(__VA_ARGS__))(__VA_ARGS__)	\
		SERVER_OPT sendError(err, FD_NAME, param_str);	\
		return;											\
	}

#define SEND_ERR_CONTINUE(err, ...)						\
	{													\
		std::string param_str;							\
		BUILD_ERR(__VA_OPT__(__VA_ARGS__))(__VA_ARGS__)	\
		SERVER_OPT sendError(err, FD_NAME, param_str);	\
		continue;										\
	}

