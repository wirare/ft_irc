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

std::string checkForSpace(std::string playerName);

typedef struct s_playerInfo {
    std::string puuid;
    std::string playerName;
    std::string playerTag;
}   t_playerInfo;

class LeagueOfBot {
    public :
        LeagueOfBot();
        ~LeagueOfBot();
        std::string getKey() {return _apiKey;};
        t_playerInfo getInfo() {return _Info;};
        std::string getKey(std::string &buffer, const std::string &key);
        void setPuuidInfo(std::string puuid) {_Info.puuid = puuid;};
        void getPuuid(void);
        void getAllMastery();
    private :
        size_t _pos;
        t_playerInfo _Info;
        std::string _apiKey;
        std::map<int, std::string> _champId;
};