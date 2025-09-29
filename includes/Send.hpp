/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Send.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ellanglo <ellanglo@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 20:50:36 by ellanglo          #+#    #+#             */
/*   Updated: 2025/09/29 19:16:08 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifdef SEND
#	undef SEND
#endif
#ifdef SERVER
	#define SEND(...) sendMessage(fd, buildMessage(__VA_ARGS__))
	#define SEND_ERR(err) {sendError(err, fd); return;}
#elif defined(CHANNEL)
	#define SEND(...) server.sendMessage(client.getFd(), server.buildMessage(__VA_ARGS__))
	#define SEND_ERR(err) {server.sendError(err, client.getFd()); return;}
#else
	#define SEND(...) server.sendMessage(body.client.getFd(), server.buildMessage(__VA_ARGS__))
	#define SEND_ERR(err) {server.sendError(err, body.client.getFd()); return;}
#endif
