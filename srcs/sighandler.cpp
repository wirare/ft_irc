/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sighandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ellanglo <ellanglo@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 15:55:58 by ellanglo          #+#    #+#             */
/*   Updated: 2025/12/10 16:00:42 by ellanglo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <Server.hpp>
#include <csignal>
#include <cstring>
#include <unistd.h>
#include <Signals.hpp>

volatile sig_atomic_t g_stop = 0;

void handle_signal(int sig)
{
    if (sig == SIGINT || sig == SIGTERM)
        g_stop = 1;
}

void setup_signals()
{
    struct sigaction sa;
    std::memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}
