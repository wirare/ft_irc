#pragma once
#include <string>

#define ERR(x) case x: return ERR##x

#define ERR403 "ERR_NOSUCHCHANNEL"
#define ERR433 "ERR_NICKNAMEIN_USE"
#define ERR431 "ERR_NONICKAMEGIVEN"
#define ERR442 "ERR_NOTONCHANNEL"
#define ERR461 "ERR_NEEDMOREPARAMS"
#define ERR462 "ERR_ALREADYREGISTRED"
#define ERR464 "ERR_PASSWDMISMATCH"
#define ERR471 "ERR_CHANNELISFULL"
#define ERR473 "ERR_INVITEONLYCHAN"
#define ERR475 "ERR_BADCHANNELKEY"
#define ERR476 "ERR_BADCHANMASK"
#define ERR482 "ERR_CHANOPRIVSNEEDED"

inline std::string getErrMsg(int err)
{
	switch (err)
	{
		ERR(403);
		ERR(433);
		ERR(431);
		ERR(442);
		ERR(461);
		ERR(462);
		ERR(464);
		ERR(471);
		ERR(473);
		ERR(475);
		ERR(476);
		ERR(482);
		default: return "Unknown Error";
	}
}
