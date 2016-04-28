#include "ServerSideGame.h"
#include "Server.h"

ServerSideGame::ServerSideGame() : id(0) 
{

}

ServerSideGame::ServerSideGame(RakNet::RakPeerInterface& m_pPeerInterface, int a_id, ConnectionInfo a_playerA, ConnectionInfo a_playerB) : id(a_id)
{
	players.push_back(a_playerA);
	players.push_back(a_playerB);
	Start(m_pPeerInterface);

}

void ServerSideGame::Start(RakNet::RakPeerInterface& pPeerInterface)
{
	for (int i = 0; i < 4; i++)
	{
		Red[i].position = glm::vec3(10 + i * 20, 1.25, 10);
		Red[i + 4].position = glm::vec3(20 + i * 20, 1.25, 20);
		Red[i + 8].position = glm::vec3(10 + i * 20, 1.25, 30);


		Blue[i].position = glm::vec3(20 + i * 20, 1.25, 60);
		Blue[i + 4].position = glm::vec3(10 + i * 20, 1.25, 70);
		Blue[i + 8].position = glm::vec3(20 + i * 20, 1.25, 80);
	}

	for (int i = 0; i < 12; i++)
	{
		Red[i].color = glm::vec4(1, 0, 0, 1);
		Blue[i].color = glm::vec4(0, 0, 1, 1);
	}
	// send data to players
//	RakNet::Packet* packet = nullptr;

	for (int i = 0; i < 2; i++)
	{
		RakNet::BitStream bsOut;
		bsOut.Write((RakNet::MessageID)GameMessages::ID_SET_UP_GAME);
		bsOut.Write(id); // game id
		bsOut.Write(players[i].uiConnectionID); // client id
		(i == 1) ? bsOut.Write(turn) : bsOut.Write(!turn);	// trun
		bsOut.Write(player); // playerturn
		for (int j = 0; j < 12; j++)
		{
			bsOut.Write(Red[j].position.x);
			bsOut.Write(Red[j].position.y);
			bsOut.Write(Red[j].position.z);
			bsOut.Write(Red[j].isALive);
			bsOut.Write(Red[j].crowned);

			bsOut.Write(Blue[j].position.x);
			bsOut.Write(Blue[j].position.y);
			bsOut.Write(Blue[j].position.z);
			bsOut.Write(Blue[j].isALive);
			bsOut.Write(Blue[j].crowned);
		}// checker positions
		pPeerInterface.Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, players[i].sysAddress, false);
	}

	Update(pPeerInterface);
}

void ServerSideGame::Update(RakNet::RakPeerInterface& m_pPeerInterface)
{
	// recieve the data 
	// do the action with shit 
	// tell who can move now
	RakNet::Packet* packet = nullptr;
	while (true)
	{
		for (packet = m_pPeerInterface.Receive(); packet; m_pPeerInterface.DeallocatePacket(packet), packet = m_pPeerInterface.Receive())
		{
			switch (packet->data[0])
			{
			case ID_SEND_CURRENT_MOVE:
			{
				std::cout << "HOLYSHIT.\n";
				break;
			}
			default:
				break;
			}
		}
	}





}