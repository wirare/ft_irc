#pragma once

#include <string>
#include <ostream>
#include <DeclMacro.hpp>
#include <vector>
#include <algorithm>

typedef enum {
	NEW,
	POST_PASS,
	AUTH,
}	State;

class Channel;

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
		bool operator<(const Client &other) const { return fd < other.fd; }
		bool operator==(const Client &other) const { return fd == other.fd; }
		void forwardMessage(const std::string &msg) const;
		void addChannel(const Channel &chan) { channelList.push_back(chan); }
		void delChannel(const Channel &chan) { channelList.erase(std::remove(channelList.begin(), channelList.end(), chan), channelList.end()); }
		const std::vector<Channel> getChannels() {return channelList; }
	
	private:
		std::vector<Channel> channelList;
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
