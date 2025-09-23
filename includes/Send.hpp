/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Send.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ellanglo <ellanglo@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 20:50:36 by ellanglo          #+#    #+#             */
/*   Updated: 2025/09/23 21:00:54 by ellanglo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifdef SEND
#undef SEND
#endif
#ifdef SERVER
	#define SEND(...) sendMessage(fd, buildMessage(__VA_ARGS__))
#else
	#define SEND(...) server.sendMessage(body.client.getFd(), server.buildMessage(__VA_ARGS__))
#endif
