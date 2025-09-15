#include <Command.hpp>
#include <iostream>
#include <string>

#define buildCmd(id)				\
	void cmd##id() { std::cout << "Command "#id" got called\n"; }

#define CASE(id) case id: cmd##id(); break;

buildCmd(NICK);
buildCmd(USER);
buildCmd(JOIN);
buildCmd(PART);
buildCmd(PRIVMSG);
buildCmd(QUIT);
buildCmd(KICK);
buildCmd(INVITE);
buildCmd(TOPIC);
buildCmd(MODE);
buildCmd(UNKNOWN);

void doCommand(CommandId id)
{
	switch (id)
	{
		CASE(NICK)
		CASE(USER)
		CASE(JOIN)
		CASE(PART)
		CASE(PRIVMSG)
		CASE(QUIT)
		CASE(KICK)
		CASE(INVITE)
		CASE(TOPIC)
		CASE(MODE)
		CASE(UNKNOWN)
	}
}

