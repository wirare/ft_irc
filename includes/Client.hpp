#pragma once

#include <string>
#include <vector>
#include <ostream>

typedef enum {
	NEW,
	POST_PASS,
	AUTH,
}	State;

#define DECLARE(type, Var)									\
	public:													\
		inline void set##Var(const type &v) { Var = v; };	\
		inline type get##Var() const { return Var; };		\
	private:												\
		type Var											\

class Client
{
	DECLARE(std::string, Nick);
	DECLARE(std::string, Username);
	DECLARE(std::string, Realname);
	DECLARE(State, State);
	DECLARE(std::string, LastPass);
	DECLARE(bool, SendPass);

	public:
		Client(int fd): Nick("UNSET"), Username("UNSET"), Realname("UNSET"), State(NEW), LastPass(""), SendPass(false), fd(fd) {};
		int	getFd() const { return fd; };
	
	private:
		int fd;
		std::vector<int> channels;
};

#define STATE(x) case(x) : return #x

inline std::string state_to_str(State state)
{
	switch (state)
	{
		STATE(NEW);
		STATE(POST_PASS);
		STATE(AUTH);
	}
}

#undef STATE

inline std::ostream &operator<<(std::ostream &oss, const Client &client)
{
	oss << "Client " << client.getFd() << " informations:\n";
	oss << "Nick: " << client.getNick() << "\n";
	oss << "Username: " << client.getUsername() << "\n";
	oss << "Realname: " << client.getRealname() << "\n";
	oss << "State: " << state_to_str(client.getState()) << "\n";
	return oss;
}
