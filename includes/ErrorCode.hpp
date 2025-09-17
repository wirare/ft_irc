#pragma once
#include <string>

#define ERR(x) case x: return ERR##x

#define ERR433 "ERR_NICKNAMEIN_USE"
#define ERR431 "ERR_NONICKAMEGIVEN"
#define ERR461 "ERR_NEEDMOREPARAMS"
#define ERR462 "ERR_ALREADYREGISTRED"
#define ERR464 "ERR_PASSWDMISMATCH"

inline std::string getErrMsg(int err)
{
	switch (err)
	{
		ERR(433);
		ERR(431);
		ERR(461);
		ERR(462);
		ERR(464);
		default: return "Unknown Error";
	}
}

#define SEND_ERR(err)								\
	{server.sendError(err, body.client.getFd());	\
	return;}										\
