#pragma once

#include <netinet/in.h>
#include <string>
#include <ostream>
#include <DeclMacro.hpp>
#include <ATarget.hpp>
#include <unistd.h>
#include <arpa/inet.h>

typedef enum {
	NEW,
	POST_PASS,
	AUTH,
}	State;

class Channel;

class Client: public ATarget
{
	DECLARE(std::string, Nick);
	DECLARE(std::string, Username);
	DECLARE(std::string, Realname);
	DECLARE(State, State);
	DECLARE(std::string, LastPass);
	DECLARE(bool, SendPass);
	DECLARE(std::string, Hostname);

	public:
		Client() {};
		Client(int fd): Nick("UNSET"), Username("UNSET"), Realname("UNSET"), State(NEW), LastPass(""), SendPass(false), fd(fd) 
		{
			struct sockaddr_in addr;
			Hostname = inet_ntoa(addr.sin_addr);
		};
		inline int	getFd() const { return fd; };
		inline bool operator<(const Client &other) const { return fd < other.fd; }
		inline bool operator==(const Client &other) const { return fd == other.fd; }
		inline bool operator!=(const Client &other) const { return fd != other.fd; }
		void recvMessage(Client *client, const std::string &msg) const;
	
	private:
		int fd;
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
