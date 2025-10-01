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

void getPuuid(LeagueOfBot bot) {
    std::string buffer; 
    std::string curlUrl = "curl -s \'https://" + bot.getInfo().region + ".api.riotgames.com/riot/account/v1/accounts/by-riot-id/" + checkForSpace(bot.getInfo().playerName) + "/" + bot.getInfo().playerTag +  "?api_key=" + bot.getKey() + "\'";

    FILE* pipe(popen(curlUrl.c_str(), "r"));
    if (!pipe) {
        std::cerr << "Impossible d'ouvrir le pipe curl\n";
        return;
    }
    char chunk[128];
    while (fgets(chunk, sizeof(chunk), pipe) != NULL) {
        buffer += chunk;
    }
    pclose(pipe);

    std::cout << buffer;
}


int main()
{
    try {
        LeagueOfBot bot;
        getPuuid(bot);
    } catch (std::runtime_error &e) {
        std::cout << e.what() << "\n";
    }
}
