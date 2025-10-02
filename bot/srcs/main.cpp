#include <LeagueOfBot.hpp>

// a quoi ressemble l'api : "https://{region}.api.riotgames.com/riot/{type de l'api}/{la version}/{la methode de l'api}/{les parametres}?{la cle api}"

std::string checkForSpace(std::string playerName) {
    std::string urlPlayerName;

    for (size_t i = 0; i < playerName.size(); i++) {
        if (playerName[i] == ' ')
            urlPlayerName.append("%20");
        else
            urlPlayerName.push_back(playerName[i]);
    }
    return urlPlayerName;
}

int main()
{
    try {
        LeagueOfBot bot;
        bot.getPuuid();
        bot.getAllMastery();
    } catch (std::runtime_error &e) {
        std::cout << e.what() << "\n";
    }
}
