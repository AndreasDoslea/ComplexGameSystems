#pragma once
#include "Checker.h"
#include "Server.h"

class ServerSideGame : public Server {
public:
	ServerSideGame();
	ServerSideGame(RakNet::RakPeerInterface& m_pPeerInterface, int a_id, ConnectionInfo a_playerA, ConnectionInfo a_playerB);
	void Start(RakNet::RakPeerInterface& m_pPeerInterface);

	std::vector<int> data;

private:

	glm::vec3	m_pickPosition;
	int selectedChecker;
	Checker tempChecker;

	bool turn;

	bool serverTurnSwap = false;

	std::vector<ConnectionInfo> players;

	int id;

	void Update(RakNet::RakPeerInterface& m_pPeerInterface);
	void Move(RakNet::BitStream &bsIn, RakNet::RakPeerInterface& m_pPeerInterface);
	int findChecker(Checker checker);

	std::vector<Directions> CheckMoveableDirections(Checker currentPlayer[], Checker otherPlayer[], const Checker &selected);
	int findCheckerToDie(Directions dir, Checker currentPlayer[], Checker otherPlayer[], const Checker &selected);


	enum Player { Player_1, Player_2 };
	enum State { Clicked, Moved, Empty };

	Player player = Player_1;
	State state = Empty;


	void Movement(glm::vec3 pickPosition, Checker currentPlayer[], Checker otherPlayer[], int selectedChecker, State &state, glm::vec4 colour, RakNet::RakPeerInterface& m_pPeerInterface);
	void Jumping(glm::vec3 pickPosition, Checker currentPlayer[], Checker otherPlayer[], int selectedChecker, State &state, glm::vec4 colour, RakNet::RakPeerInterface& m_pPeerInterface);
	void RecursiveJumping(Checker currentPlayer[], Checker otherPlayer[], glm::vec4 colour, int selectedChecker);

	bool DidBlueCrown(Checker currentPlayer[]);
	bool DidRedCrown(Checker currentPlayer[]);

	void SendData(RakNet::RakPeerInterface& m_pPeerInterface);

	Checker Red[12];
	Checker Blue[12];

};