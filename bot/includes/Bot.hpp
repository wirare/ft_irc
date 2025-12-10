#pragma once

#include <iostream>
#include <cstring>
#include <netdb.h>
#include <unistd.h>

typedef struct s_userInfo {
    std::string Name;
    std::string Cmd;
}   t_userInfo;