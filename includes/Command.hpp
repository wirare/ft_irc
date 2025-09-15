#pragma once

typedef enum {
	UNKNOWN,
	NICK,
	USER,
	JOIN,
	PART,
	PRIVMSG,
	QUIT,
	KICK,
	INVITE,
	TOPIC,
	MODE
}	CommandId;

void doCommand(CommandId id);
