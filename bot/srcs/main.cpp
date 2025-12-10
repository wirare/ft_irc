#include <Bot.hpp>


int main(int argc, char **argv)
{
    const char* server = "127.0.0.1";

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));

    struct hostent* host = gethostbyname(server);
    if (!host) {
        return 1;
   }
    serv_addr.sin_addr = *((struct in_addr*)host->h_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        close(sock);
        perror("connect");
        return 1;
    }

    std::string passMsg = "PASS " + std::string(argv[2]) + "\r\n";
    send(sock, passMsg.c_str(), passMsg.size(), 0);

    std::string msg = "NICK " + std::string("NumberBot") + "\r\nUSER " + "NumberBot" + " 0 * :" + "NumberBot" + "\r\n";
    send(sock, msg.c_str(), msg.size(), 0);

    char buffer[512];
    srand(time(nullptr));

    while (true)
    {
        std::memset(buffer, 0, sizeof(buffer));
        int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            std::cout << "Disconnected from server\n";
            break;
        }
        buffer[bytes] = '\0';

        std::string msg(buffer);

        if (msg.find("PING") == 0) {
            std::string pong = "PONG" + msg.substr(4) + "\r\n";
            send(sock, pong.c_str(), pong.size(), 0);
        }

        if (msg.rfind(":", 0) == 0)
        {
            size_t startNick = 1;
            size_t endNick = msg.find(' ');
            std::string author = msg.substr(startNick, endNick - startNick);

            size_t priv = msg.find("PRIVMSG ");
            if (priv == std::string::npos)
                continue;

            size_t startTarget = priv + 8;
            size_t endTarget = msg.find(' ', startTarget);

            std::string content = msg.substr(endTarget + 1);

            while (!content.empty() && (content.back() == '\r' || content.back() == '\n'))
                content.pop_back();

            if (content == "!random")
            {
                int randomnumber = rand() % 101;

                std::string replyTarget = author;

                std::string response = "PRIVMSG " + replyTarget + " :Random number = " + std::to_string(randomnumber) + "\r\n";
            
                send(sock, response.c_str(), response.size(), 0);
            }
        }
    }
    close(sock);
}