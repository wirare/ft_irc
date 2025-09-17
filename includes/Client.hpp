#pragma once

#include <string>
#include <vector>

#define DECLARE(type, var, Var)						\
	public:											\
		void set##Var(const type &v) { var = v; };	\
		type get##Var() const { return var; };		\
	private:										\
		type var									\

typedef enum {
	CONNECTED,
	AUTH,
	WAITING,
}	State;

class Client
{
	DECLARE(std::string, nick, Nick);
	DECLARE(std::string, username, Username);
	DECLARE(std::string, realname, Realname);
	DECLARE(State, state, State);
	public:
		Client(int fd): nick("UNSET"), username("UNSET"), realname("UNSET"), fd(fd) {};
		int getFd() const { return fd; };
	
	private:
		int fd;
		std::vector<int> channels;
};
