/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Send.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ellanglo <ellanglo@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 20:50:36 by ellanglo          #+#    #+#             */
/*   Updated: 2025/10/01 23:54:18 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef USEFULL_MACRO
	#define USEFULL_MACRO
	#define	_CONCAT(a, b)			a ## b
	#define	CONCAT(a, b)			_CONCAT(a, b)
	#define RETURN_2				continue;
	#define	RETURN_1	
	#define	RETURN_0				return;
	#define	_COUNT(_a, _b, _N, ...)	_N
	#define	COUNT(...)				_COUNT(__VA_ARGS__ __VA_OPT__(,) 2, 1, 0)
	#define	RETURN(...)				CONCAT(RETURN_, COUNT(__VA_ARGS__))
	#define	NORETURN				0
	#define CONTINUE				0,0
#endif

#ifdef SEND
#	undef SEND
#	undef SEND_ERR
#endif

#if defined(SERVER)
	#define SEND(...) sendMessage(fd, buildMessage(__VA_ARGS__))
	#define SEND_ERR(err) {sendError(err, fd); return;}
#elif defined(CHANNEL)
	#define SEND(...) server.sendMessage(client.getFd(), server.buildMessage(__VA_ARGS__))
	#define SEND_ERR(err) {server.sendError(err, client.getFd()); return;}
#elif defined(CLIENT)
	#define SEND(...) server.sendMessage(fd, server.buildMessage(__VA_ARGS__))
#else
	#define SEND(...) server.sendMessage(body.client.getFd(), server.buildMessage(__VA_ARGS__))
	#define SEND_ERR(err, ...) {server.sendError(err, body.client.getFd()); RETURN(__VA_OPT__(__VA_ARGS__))}
#endif
