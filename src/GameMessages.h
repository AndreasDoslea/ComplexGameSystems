#pragma once

#include <MessageIdentifiers.h> 
#include "Checker.h"

enum GameMessages {
	ID_SERVER_TEXT_MESSAGE = ID_USER_PACKET_ENUM + 1,
	ID_SERVER_CLIENT_ID = ID_USER_PACKET_ENUM + 2,
	ID_CLIENT_CREATE_OBJECT = ID_USER_PACKET_ENUM + 3,
	ID_SERVER_FULL_OBJECT_DATA = ID_USER_PACKET_ENUM + 4,
	ID_CLIENT_UPDATE_OBJECT_POSITION = ID_USER_PACKET_ENUM + 5,
	ID_START_GAME = ID_USER_PACKET_ENUM + 6,
	ID_WAIT_IN_LOBBY = ID_USER_PACKET_ENUM + 7,
	ID_SET_UP_GAME = ID_USER_PACKET_ENUM + 8,
	ID_SEND_CURRENT_MOVE = ID_USER_PACKET_ENUM + 9,
	ID_SERVER_DECISION = ID_USER_PACKET_ENUM + 10,
	ID_SERVER_UPDATE = ID_USER_PACKET_ENUM + 11,
};

enum Player { Player_1, Player_2 };

struct GameData
{
	unsigned int gameId;
	unsigned int playerId;
	Player playerTurn;
	Checker checker;
	glm::vec3 pickPosition;
};

struct ConnectionInfo
{
	unsigned int			uiConnectionID;
	RakNet::SystemAddress	sysAddress;
};