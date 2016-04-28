#pragma once
#include "Checker.h"
#include "Server.h"

class ServerSideGame : public Server {
public:
	ServerSideGame();
	ServerSideGame(RakNet::RakPeerInterface& m_pPeerInterface, int a_id, ConnectionInfo a_playerA, ConnectionInfo a_playerB);
	void Start(RakNet::RakPeerInterface& m_pPeerInterface);


private:

	glm::vec3	m_pickPosition;
	int selectedChecker;

	bool turn;

	std::vector<ConnectionInfo> players;

	int id;

	void Update(RakNet::RakPeerInterface& m_pPeerInterface);


	enum Player { Player_1, Player_2 };
	enum State { Clicked, Moved, Empty };

	Player player = Player_1;
	State state = Empty;

	Checker Red[12];
	Checker Blue[12];

};