#pragma once
#include <iostream>
#include <string>

#include <thread>
#include <chrono>
#include <unordered_map>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

#include <algorithm>

#include "GameMessages.h"
#include "GameObject.h"

#include "Checker.h"

#include "ServerSideGame.h"



class Server {
public:



	Server();
	~Server();

	void run();

protected:

	void handleNetworkMessages();
	unsigned int systemAddressToClientID(RakNet::SystemAddress& systemAddress);
	RakNet::SystemAddress& clientIDToSystemAddress(unsigned int a_clientID);

	// connection functions
	void addNewConnection(RakNet::SystemAddress systemAddress);
	void removeConnection(RakNet::SystemAddress systemAddress);

	void sendClientIDToClient(unsigned int uiClientID);
	void createNewObject(RakNet::BitStream& bsIn, RakNet::SystemAddress& ownerSysAddress);
	void sendGameObjectToAllClients(GameObject& gameObject, RakNet::SystemAddress ownerSystemAddress);
	void sendGameObjectsToAll();

	void updateLobby(RakNet::BitStream& bsIn, RakNet::SystemAddress ownerSystemAddress);
	void lobby();
	void StartGame();

	void ReadAndSendData(RakNet::BitStream& bsIn);

	RakNet::RakPeerInterface*							m_pPeerInterface;

private:

	GameData tempGameData;

	const unsigned short PORT = 5456;


	unsigned int										m_uiConnectionCounter;
	std::unordered_map<unsigned int, ConnectionInfo>	m_connectedClients;
	std::vector<GameObject>								gameObjects;
	std::vector<ConnectionInfo>							m_peopleInLobby;
	std::vector<ConnectionInfo>							m_totalPeopleConnected;
	std::vector<std::thread>							m_threads;
	std::vector<ServerSideGame>							m_games;
	unsigned int										m_uiObjectCounter;
	int gameCount = 0;
};