/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UsefullMacro.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wirare <wirare@42angouleme.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 15:03:51 by wirare            #+#    #+#             */
/*   Updated: 2025/10/10 13:13:14 by wirare           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef USEFULL_MACRO
	#define USEFULL_MACRO
	#define	_CONCAT(a, b)			a ## b
	#define	CONCAT(a, b)			_CONCAT(a, b)
	#define	_COUNT(_a, _b, _c, _N, ...)	_N
	#define	COUNT(...)				_COUNT(__VA_ARGS__ __VA_OPT__(,) 3, 2, 1, 0)
#endif
