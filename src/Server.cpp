#include "Server.h"
#include "ServerSideGame.h"

Server::Server() {
	// initialize the Raknet peer interface first
	m_pPeerInterface = RakNet::RakPeerInterface::GetInstance();

	m_uiConnectionCounter = 1;
	m_uiObjectCounter = 1;
}

Server::~Server() {

}

void Server::run() {

	// startup the server, and start it listening to clients
	std::cout << "Starting up the server..." << std::endl;



	// create a socket descriptor to describe this connection
	RakNet::SocketDescriptor sd(PORT, 0);

	// now call startup - max of 32 connections, on the assigned port
	m_pPeerInterface->Startup(32, &sd, 1);
	m_pPeerInterface->SetMaximumIncomingConnections(32);

	handleNetworkMessages();
}

void Server::handleNetworkMessages() {
	RakNet::Packet* packet = nullptr;

	while (true)
	{
		for (packet = m_pPeerInterface->Receive(); packet; m_pPeerInterface->DeallocatePacket(packet), packet = m_pPeerInterface->Receive())
		{
			switch (packet->data[0])
			{
				case ID_NEW_INCOMING_CONNECTION:
				{
					addNewConnection(packet->systemAddress);
					std::cout << "A connection is incoming.\n";
					break;
				}
				case ID_DISCONNECTION_NOTIFICATION:
					std::cout << "A client has disconnected.\n";
					removeConnection(packet->systemAddress);
					break;
				case ID_CONNECTION_LOST:
					std::cout << "A client lost the connection.\n";
					removeConnection(packet->systemAddress);
					break;
				case ID_WAIT_IN_LOBBY:
				{
					std::cout << "A Client is Waiting in the Game LOBBY.\n";
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					updateLobby(bsIn, packet->systemAddress);
					break;
				}
				case ID_CLIENT_CREATE_OBJECT:
				{
					//here is where start game should be
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					createNewObject(bsIn, packet->systemAddress);
					break;
				}
				case ID_CLIENT_UPDATE_OBJECT_POSITION:
				{
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					int clientID = systemAddressToClientID(packet->systemAddress);
					bsIn.Read(gameObjects[clientID - 1].fXPos);
					bsIn.Read(gameObjects[clientID - 1].fZPos);
					sendGameObjectToAllClients(gameObjects[clientID - 1], packet->systemAddress);
					break;
				}
				default:
					std::cout << "Received a message with a unknown id: " << packet->data[0];
					break;
			}
		}

		lobby();

		for (int i = 0; i < gameObjects.size(); i++)
		{
			std::cout << "Game object : " << i + 1 << " at Position: " << gameObjects[i].fZPos << std::endl;
		}
		std::cout << std::endl;
	}
}

void Server::addNewConnection(RakNet::SystemAddress systemAddress) {
	ConnectionInfo info;
	info.sysAddress = systemAddress;
	info.uiConnectionID = m_uiConnectionCounter++;
	m_connectedClients[info.uiConnectionID] = info;

	sendClientIDToClient(info.uiConnectionID);
}

void Server::removeConnection(RakNet::SystemAddress systemAddress) {
	for (auto it = m_connectedClients.begin(); it != m_connectedClients.end(); it++) {
		if (it->second.sysAddress == systemAddress) {
			m_connectedClients.erase(it);
			break;
		}
	}
}

unsigned int Server::systemAddressToClientID(RakNet::SystemAddress& systemAddress) {
	for (auto it = m_connectedClients.begin(); it != m_connectedClients.end(); it++) {
		if (it->second.sysAddress == systemAddress) {
			return it->first;
		}
	}

	return 0;
}

RakNet::SystemAddress& Server::clientIDToSystemAddress(unsigned int a_clientID)
{
	for (auto it = m_connectedClients.begin(); it != m_connectedClients.end(); it++)
	{
		if (it->first == a_clientID)
		{
			return it->second.sysAddress;
		}
	}
}

void Server::sendClientIDToClient(unsigned int uiClientID) {
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)GameMessages::ID_SERVER_CLIENT_ID);
	bs.Write(uiClientID);

	m_pPeerInterface->Send(&bs, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, m_connectedClients[uiClientID].sysAddress, false);
}

void Server::createNewObject(RakNet::BitStream& bsIn, RakNet::SystemAddress& ownerSysAddress)
{
	GameObject newGameObject;

	//Read in the information from the packet
	bsIn.Read(newGameObject.fXPos);
	bsIn.Read(newGameObject.fZPos);
	bsIn.Read(newGameObject.fRedColour);
	bsIn.Read(newGameObject.fGreenColour);
	bsIn.Read(newGameObject.fBlueColour);

	newGameObject.uiOwnerClientID = systemAddressToClientID(ownerSysAddress);
	newGameObject.uiObjectID = m_uiObjectCounter++;

	gameObjects.push_back(newGameObject);

	sendGameObjectsToAll();
}

void Server::sendGameObjectToAllClients(GameObject& gameObject, RakNet::SystemAddress ownerSystemAddress)
{
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)GameMessages::ID_SERVER_FULL_OBJECT_DATA);
	bsOut.Write(gameObject.fXPos);
	bsOut.Write(gameObject.fZPos);
	bsOut.Write(gameObject.fRedColour);
	bsOut.Write(gameObject.fGreenColour);
	bsOut.Write(gameObject.fBlueColour);
	bsOut.Write(gameObject.uiOwnerClientID);
	bsOut.Write(gameObject.uiObjectID);
	m_pPeerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, ownerSystemAddress, true);
}

void Server::sendGameObjectsToAll()
{
	//LOOP THROUGH ALL GAME OBJECTS
	for (int i = 0; i < gameObjects.size(); i++)
	{
		RakNet::BitStream bsOut;

		//WRITE THE ID
		bsOut.Write((RakNet::MessageID)GameMessages::ID_SERVER_FULL_OBJECT_DATA);

		//WRITE THE GAME OBJECT DATA
		bsOut.Write(gameObjects[i].fXPos);
		bsOut.Write(gameObjects[i].fZPos);
		bsOut.Write(gameObjects[i].fRedColour);
		bsOut.Write(gameObjects[i].fGreenColour);
		bsOut.Write(gameObjects[i].fBlueColour);
		bsOut.Write(gameObjects[i].uiOwnerClientID);
		bsOut.Write(gameObjects[i].uiObjectID);

		//BROADCAST THE OBJECT TO CONNECTED CLIENTS
		m_pPeerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
	}
}

void Server::updateLobby(RakNet::BitStream& bsIn, RakNet::SystemAddress ownerSystemAddress)
{
	ConnectionInfo connectionInfo;
	bsIn.Read(connectionInfo.uiConnectionID);
	connectionInfo.sysAddress = ownerSystemAddress;
	unsigned int tempClientID;
	//if (std::find(m_totalPeopleConnected.begin(), m_totalPeopleConnected.end(), connectionInfo) != m_totalPeopleConnected.end()) {
	//	/* v contains x */
	//}
	//else 
	//{
		m_totalPeopleConnected.push_back(connectionInfo);
	//}
}

void Server::lobby()
{
	if (m_totalPeopleConnected.size() >= 2)
	{
		//std::cout << "GAME" << std::endl;
		//for (int i = 0; i < 2; i++)
		//{
		//	RakNet::BitStream bsOut;
		//	bsOut.Write((RakNet::MessageID)GameMessages::ID_START_GAME);
		//	bsOut.Write(gameCount); // game id
		//	bsOut.Write(m_totalPeopleConnected[i].uiConnectionID); // client id
		//	m_pPeerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_totalPeopleConnected[i].sysAddress, true);
		//}
		ServerSideGame Game = ServerSideGame(*m_pPeerInterface, gameCount, m_totalPeopleConnected[0], m_totalPeopleConnected[1]);
		gameCount++;
	}
	m_peopleInLobby = m_totalPeopleConnected;
}
