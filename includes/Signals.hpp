/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Signals.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ellanglo <ellanglo@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 15:59:05 by ellanglo          #+#    #+#             */
/*   Updated: 2025/12/10 16:11:53 by ellanglo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#pragma once
#include <csignal>

extern volatile sig_atomic_t g_stop;
void setup_signals();
