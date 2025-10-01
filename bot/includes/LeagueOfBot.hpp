#pragma once

#include <map>
#include <iostream>
#include <fstream>
#include <exception>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <cstdio>
#include <sstream>

typedef struct s_playerInfo {
    std::string puuid;
    std::string playerName;
    std::string playerTag;
    std::string region;
}   t_playerInfo;

class LeagueOfBot {
    public :
        LeagueOfBot();
        ~LeagueOfBot();
        std::string getKey() {return _apiKey;};
        t_playerInfo getInfo() {return _Info;};
    private :
        t_playerInfo _Info;
        std::string _apiKey;
        std::map<int, std::string> _champId;
};