#pragma once
#include "Checker.h"
//#include "Server.h"
#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

#include "GameMessages.h"


#include <thread>

class ServerSideGame{
public:
	ServerSideGame();
	ServerSideGame(RakNet::RakPeerInterface* m_pPeerInterface, int a_id, ConnectionInfo a_playerA, ConnectionInfo a_playerB);
	void Update();
	std::vector<GameData> gameData;

private:
	RakNet::RakPeerInterface* m_pPeerInterface;

	glm::vec3	m_pickPosition;
	int selectedChecker;
	Checker tempChecker;

	bool turn;

	bool serverTurnSwap = false;

	std::vector<ConnectionInfo> players;

	int id;

	void Start();
	void Move();
	int findChecker(Checker checker);

	std::vector<Directions> CheckMoveableDirections(Checker currentPlayer[], Checker otherPlayer[], const Checker &selected);
	int findCheckerToDie(Directions dir, Checker currentPlayer[], Checker otherPlayer[], const Checker &selected);


	enum State { Clicked, Moved, Empty };

	Player player = Player_1;
	State state = Empty;


	void Movement(glm::vec3 pickPosition, Checker currentPlayer[], Checker otherPlayer[], int selectedChecker, State &state, glm::vec4 colour);
	void Jumping(glm::vec3 pickPosition, Checker currentPlayer[], Checker otherPlayer[], int selectedChecker, State &state, glm::vec4 colour);
	void RecursiveJumping(Checker currentPlayer[], Checker otherPlayer[], glm::vec4 colour, int selectedChecker);

	bool DidBlueCrown(Checker currentPlayer[]);
	bool DidRedCrown(Checker currentPlayer[]);

	void SendData();

	Checker Red[12];
	Checker Blue[12];

};