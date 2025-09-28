/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeclMacro.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ellanglo <ellanglo@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 17:09:55 by ellanglo          #+#    #+#             */
/*   Updated: 2025/09/28 17:15:00 by ellanglo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#pragma once

#define DECLARE(type, Var)									\
	public:													\
		inline void set##Var(const type &v) { Var = v; };	\
		inline type get##Var() const { return Var; };		\
	private:												\
		type Var											\
