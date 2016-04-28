#pragma once

#include <MessageIdentifiers.h>

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
};