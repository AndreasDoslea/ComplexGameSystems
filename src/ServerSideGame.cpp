#include "ServerSideGame.h"
#include "Server.h"
#include "Checker.h"

#include <atomic>

std::atomic<bool> threadStop = false;

ServerSideGame::ServerSideGame() : id(0) 
{

}

ServerSideGame::ServerSideGame(RakNet::RakPeerInterface* pPeerInterface, int a_id, ConnectionInfo a_playerA, ConnectionInfo a_playerB) : id(a_id)
{
	m_pPeerInterface = pPeerInterface;
	players.push_back(a_playerA);
	players.push_back(a_playerB);
	Start();

}

void ServerSideGame::Start()
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
		Red[i].type = RedType;
		Blue[i].type = BlueType;
	}
	// send data to players
//	RakNet::Packet* packet = nullptr;

	for (int i = 0; i < 2; i++)
	{
		RakNet::BitStream bsOut;
		bsOut.Write((RakNet::MessageID)GameMessages::ID_SET_UP_GAME);
		bsOut.Write(id); // game id
		bsOut.Write(players[i].uiConnectionID); // client id
		(i == serverTurnSwap) ? bsOut.Write(turn) : bsOut.Write(!turn);	// trun
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
		m_pPeerInterface->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, players[i].sysAddress, false);
	}

	Update();
}

void ServerSideGame::Update()
{
	// recieve the data 
	// do the action with shit 
	// tell who can move now
	//RakNet::Packet* packet = nullptr;
	//while (!threadStop.load())
	//{
		//for (packet = m_pPeerInterface.Receive(); packet; m_pPeerInterface.DeallocatePacket(packet), packet = m_pPeerInterface.Receive())
		//{
		//	switch (packet->data[0])
		//	{
		//	case ID_SEND_CURRENT_MOVE:
		//	{
		//		RakNet::BitStream bsIn(packet->data, packet->length, true);
		//		bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
		//		Move(gameData, m_pPeerInterface);
		//		break;
		//	}
		//	case ID_NEW_INCOMING_CONNECTION:
		//	{
		//		std::cout << "SHIT" << std::endl;
		//	}
		//	//case ASKFAKL:
		//	//{
		//	//	threadStop.store(true);
		//	//}
		//	default:
		//		break;
		//	}
		//}
	
		if (gameData.size() > 0)
		{
			std::thread myThread(&ServerSideGame::Move, this);
			myThread.join();
			gameData.clear();
		}
	//}
}

void ServerSideGame::Move()
{
	unsigned int playerid;
	int RecieveGameId;
	//bsIn.Read(RecieveGameId); // game ID
	//if (!RecieveGameId == id)
	//{
	//	return;
	//}
	//bsIn.Read(playerid); // client
	//bsIn.Read(player); // Persons Turn
	//bsIn.Read(tempChecker.position.x); // Checker
	//bsIn.Read(tempChecker.position.y); // Checker
	//bsIn.Read(tempChecker.position.z); // Checker
	//bsIn.Read(m_pickPosition.x); // PickPosition
	//bsIn.Read(m_pickPosition.y); // PickPosition
	//bsIn.Read(m_pickPosition.z); // PickPosition

	for (int i = 0; i < gameData.size(); i++)
	{
		RecieveGameId = gameData[i].gameId;

		if (!RecieveGameId == id)
		{
			//gameData.erase(gameData.begin());
			return;
		}

		playerid = gameData[i].playerId;
		player = gameData[i].playerTurn;
		tempChecker.position.x = gameData[i].checker.position.x;
		tempChecker.position.y = gameData[i].checker.position.y;
		tempChecker.position.z = gameData[i].checker.position.z;
		m_pickPosition.x = gameData[i].pickPosition.x;
		m_pickPosition.y = gameData[i].pickPosition.y;
		m_pickPosition.z = gameData[i].pickPosition.z;

		//gameData.erase(gameData.begin());
	}

	

	selectedChecker = findChecker(tempChecker);

	for (int i = 0; i < 12; i++)
	{
		Red[i].canJump = false;
		Blue[i].canJump = false;
		Red[i].directions = CheckMoveableDirections(Red, Blue, Red[i]);
		for (int j = 0; j < Red[i].directions.size(); j++)
		{
			if (Red[i].crowned)
			{
				if (Red[i].directions[j] == OneJump ||
					Red[i].directions[j] == TwoJump ||
					Red[i].directions[j] == ThreeJump ||
					Red[i].directions[j] == FourJump)
				{
					Red[i].canJump = true;
				}
			}
			else
			{
				if (Red[i].directions[j] == OneJump ||
					Red[i].directions[j] == TwoJump)
				{
					Red[i].canJump = true;
				}
			}
		}
		Blue[i].directions = CheckMoveableDirections(Blue, Red, Blue[i]);
		for (int j = 0; j < Blue[i].directions.size(); j++)
		{
			if (Blue[i].crowned)
			{
				if (Blue[i].directions[j] == OneJump ||
					Blue[i].directions[j] == TwoJump ||
					Blue[i].directions[j] == ThreeJump ||
					Blue[i].directions[j] == FourJump)
				{
					Blue[i].canJump = true;
				}
			}
			else
			{
				if (Blue[i].directions[j] == ThreeJump ||
					Blue[i].directions[j] == FourJump)
				{
					Blue[i].canJump = true;
				}
			}
		}
	}


	switch (player)
	{
	case Player_1:
	{
		int c = 0;
		for (int i = 0; i < 12; i++)
		{
			if (!Red[i].isALive)
			{
				c++;
			}
		}
		int a = 0;
		for (int i = 0; i < 12 - c; i++)
		{
			if (!Red[i].canJump)
			{
				a++;
			}
		}
		if (a == 12 - c)
		{
			Jumping(m_pickPosition, Red, Blue, selectedChecker, state, glm::vec4(1, 0, 0, 1));
			Movement(m_pickPosition, Red, Blue, selectedChecker, state, glm::vec4(1, 0, 0, 1));
		}
		if (Red[selectedChecker].canJump)
		{
			Jumping(m_pickPosition, Red, Blue, selectedChecker, state, glm::vec4(1, 0, 0, 1));
			//Movement(m_pickPosition, Red, Blue, selectedChecker, state, glm::vec4(1, 0, 0, 1), m_pPeerInterface);
		}
		break;
	}
	case Player_2:
	{
		int d = 0;
		for (int i = 0; i < 12; i++)
		{
			if (!Blue[i].isALive)
			{
				d++;
			}
		}
		int b = 0;
		for (int i = 0; i < 12 - d; i++)
		{
			if (!Blue[i].canJump)
			{
				b++;
			}
		}
		if (b == 12 - d)
		{
			Jumping(m_pickPosition, Blue, Red, selectedChecker, state, glm::vec4(0, 0, 1, 1));
			Movement(m_pickPosition, Blue, Red, selectedChecker, state, glm::vec4(0, 0, 1, 1));
		}

		if (Blue[selectedChecker].canJump)
		{
			Jumping(m_pickPosition, Blue, Red, selectedChecker, state, glm::vec4(0, 0, 1, 1));
			//Movement(m_pickPosition, Blue, Red, selectedChecker, state, glm::vec4(0, 0, 1, 1), m_pPeerInterface);
		}
		break;
	}
	default:
	{
		break;
	}
	}
}

int ServerSideGame::findChecker(Checker checker)
{
	for (int i = 0; i < 12; i++)
	{
		if (checker.position.x == Red[i].position.x && 
			checker.position.y == Red[i].position.y && 
			checker.position.z == Red[i].position.z)
		{
			return i;
		}
		else if (checker.position.x == Blue[i].position.x &&
				checker.position.y == Blue[i].position.y &&
				checker.position.z == Blue[i].position.z)
		{
			return i;
		}
	}
	return -1;
}

bool canMoveDirectionOne(Checker currentPlayer[], Checker otherPlayer[], const Checker &selected, int distance)
{
	for (int j = 0; j < 12; j++)
	{
		if (selected.position.x - distance < 10 || selected.position.z + distance > 80)
		{
			return false;
		}
		else
		{
			if ((currentPlayer[j].position.x > selected.position.x - 5 - distance &&
				currentPlayer[j].position.x < selected.position.x + 5 - distance &&
				currentPlayer[j].position.z > selected.position.z - 5 + distance &&
				currentPlayer[j].position.z < selected.position.z + 5 + distance) ||
				(otherPlayer[j].position.x > selected.position.x - 5 - distance &&
					otherPlayer[j].position.x < selected.position.x + 5 - distance &&
					otherPlayer[j].position.z > selected.position.z - 5 + distance &&
					otherPlayer[j].position.z < selected.position.z + 5 + distance))
			{
				return false;
			}
		}
	}
	return true;
}
bool canMoveDirectionTwo(Checker currentPlayer[], Checker otherPlayer[], const Checker &selected, int distance)
{
	for (int j = 0; j < 12; j++)
	{
		if (selected.position.x + distance > 80 || selected.position.z + distance > 80)
		{
			return false;
		}
		else
		{
			if ((currentPlayer[j].position.x > selected.position.x - 5 + distance &&
				currentPlayer[j].position.x < selected.position.x + 5 + distance &&
				currentPlayer[j].position.z > selected.position.z - 5 + distance &&
				currentPlayer[j].position.z < selected.position.z + 5 + distance) ||
				(otherPlayer[j].position.x > selected.position.x - 5 + distance &&
					otherPlayer[j].position.x < selected.position.x + 5 + distance &&
					otherPlayer[j].position.z > selected.position.z - 5 + distance &&
					otherPlayer[j].position.z < selected.position.z + 5 + distance))
			{
				return false;
			}
		}
	}
	return true;
}
bool canMoveDirectionThree(Checker currentPlayer[], Checker otherPlayer[], const Checker &selected, int distance)
{
	for (int j = 0; j < 12; j++)
	{
		if (selected.position.x + distance > 80 || selected.position.z - distance < 10)
		{
			return false;
		}
		else
		{
			if ((currentPlayer[j].position.x > selected.position.x - 5 + distance &&
				currentPlayer[j].position.x < selected.position.x + 5 + distance &&
				currentPlayer[j].position.z > selected.position.z - 5 - distance &&
				currentPlayer[j].position.z < selected.position.z + 5 - distance) ||
				(otherPlayer[j].position.x > selected.position.x - 5 + distance &&
					otherPlayer[j].position.x < selected.position.x + 5 + distance &&
					otherPlayer[j].position.z > selected.position.z - 5 - distance &&
					otherPlayer[j].position.z < selected.position.z + 5 - distance))
			{
				return false;
			}
		}
	}
	return true;
}
bool canMoveDirectionFour(Checker currentPlayer[], Checker otherPlayer[], const Checker &selected, int distance)
{
	for (int j = 0; j < 12; j++)
	{
		if (selected.position.x - distance < 10 || selected.position.z - distance < 10)
		{
			return false;
		}
		else
		{
			if ((currentPlayer[j].position.x > selected.position.x - 5 - distance &&
				currentPlayer[j].position.x < selected.position.x + 5 - distance &&
				currentPlayer[j].position.z > selected.position.z - 5 - distance &&
				currentPlayer[j].position.z < selected.position.z + 5 - distance) ||
				(otherPlayer[j].position.x > selected.position.x - 5 - distance &&
					otherPlayer[j].position.x < selected.position.x + 5 - distance &&
					otherPlayer[j].position.z > selected.position.z - 5 - distance &&
					otherPlayer[j].position.z < selected.position.z + 5 - distance))
			{
				return false;
			}
		}
	}
	return true;
}

bool canJumpDirectionOne(Checker currentPlayer[], Checker otherPlayer[], const Checker &selected)
{
	if (canMoveDirectionOne(currentPlayer, otherPlayer, selected, 20))
	{
		for (int j = 0; j < 12; j++)
		{
			if ((otherPlayer[j].position.x > selected.position.x - 5 - 10 &&
				otherPlayer[j].position.x < selected.position.x + 5 - 10 &&
				otherPlayer[j].position.z > selected.position.z - 5 + 10 &&
				otherPlayer[j].position.z < selected.position.z + 5 + 10))
			{
				return true;
			}
		}
	}
	return false;
}
bool canJumpDirectionTwo(Checker currentPlayer[], Checker otherPlayer[], const Checker &selected)
{
	if (canMoveDirectionTwo(currentPlayer, otherPlayer, selected, 20))
	{
		for (int j = 0; j < 12; j++)
		{
			if ((otherPlayer[j].position.x > selected.position.x - 5 + 10 &&
				otherPlayer[j].position.x < selected.position.x + 5 + 10 &&
				otherPlayer[j].position.z > selected.position.z - 5 + 10 &&
				otherPlayer[j].position.z < selected.position.z + 5 + 10))
			{
				return true;
			}
		}
	}
	return false;
}
bool canJumpDirectionThree(Checker currentPlayer[], Checker otherPlayer[], const Checker &selected)
{
	if (canMoveDirectionThree(currentPlayer, otherPlayer, selected, 20))
	{
		for (int j = 0; j < 12; j++)
		{
			if ((otherPlayer[j].position.x > selected.position.x - 5 + 10 &&
				otherPlayer[j].position.x < selected.position.x + 5 + 10 &&
				otherPlayer[j].position.z > selected.position.z - 5 - 10 &&
				otherPlayer[j].position.z < selected.position.z + 5 - 10))
			{
				return true;
			}
		}
	}
	return false;
}
bool canJumpDirectionFour(Checker currentPlayer[], Checker otherPlayer[], const Checker &selected)
{
	if (canMoveDirectionFour(currentPlayer, otherPlayer, selected, 20))
	{
		for (int j = 0; j < 12; j++)
		{
			if ((otherPlayer[j].position.x > selected.position.x - 5 - 10 &&
				otherPlayer[j].position.x < selected.position.x + 5 - 10 &&
				otherPlayer[j].position.z > selected.position.z - 5 - 10 &&
				otherPlayer[j].position.z < selected.position.z + 5 - 10))
			{
				return true;
			}
		}
	}
	return false;
}

void MoveDeadCheckers(Checker currentPlayer[])
{
	for (int i = 0; i < 12; i++)
	{
		if (currentPlayer[i].isALive == false)
		{
			if (currentPlayer[i].color == glm::vec4(1, 0, 0, 1))
			{
				currentPlayer[i].position = glm::vec3(0, i, 10);
			}
			else if (currentPlayer[i].color == glm::vec4(0, 0, 1, 1))
			{
				currentPlayer[i].position = glm::vec3(90, i, 80);
			}
		}
	}
}

std::vector<Directions> ServerSideGame::CheckMoveableDirections(Checker currentPlayer[], Checker otherPlayer[], const Checker &selected)
{
	// all these checks are for the blue checker
	std::vector<Directions> directions;
	if (canMoveDirectionThree(currentPlayer, otherPlayer, selected, 10))
	{
		directions.push_back(Three);
	}
	if (canMoveDirectionFour(currentPlayer, otherPlayer, selected, 10))
	{
		directions.push_back(Four);
	}
	if (canJumpDirectionThree(currentPlayer, otherPlayer, selected))
	{
		directions.push_back(ThreeJump);
	}
	if (canJumpDirectionFour(currentPlayer, otherPlayer, selected))
	{
		directions.push_back(FourJump);
	}
	if (canMoveDirectionOne(currentPlayer, otherPlayer, selected, 10))
	{
		directions.push_back(One);
	}
	if (canMoveDirectionTwo(currentPlayer, otherPlayer, selected, 10))
	{
		directions.push_back(Two);
	}
	if (canJumpDirectionOne(currentPlayer, otherPlayer, selected))
	{
		directions.push_back(OneJump);
	}
	if (canJumpDirectionTwo(currentPlayer, otherPlayer, selected))
	{
		directions.push_back(TwoJump);
	}

	return directions;
}

void ServerSideGame::RecursiveJumping(Checker currentPlayer[], Checker otherPlayer[], glm::vec4 colour, int selectedChecker)
{

	if (currentPlayer[selectedChecker].crowned)
	{
		bool canMove = true;
		if (currentPlayer[selectedChecker].position.x - 20 < 10 || currentPlayer[selectedChecker].position.z + 20 > 80)
		{
			canMove = false;
		}
		if (canMove)
		{
			if (canJumpDirectionOne(currentPlayer, otherPlayer, currentPlayer[selectedChecker]))
			{
				currentPlayer[selectedChecker].position = glm::vec3(currentPlayer[selectedChecker].position.x - 20, 1.25, currentPlayer[selectedChecker].position.z + 20);
				currentPlayer[selectedChecker].color = colour;
				otherPlayer[findCheckerToDie(OneJump, currentPlayer, otherPlayer, currentPlayer[selectedChecker])].isALive = false;
				MoveDeadCheckers(otherPlayer);
				RecursiveJumping(currentPlayer, otherPlayer, colour, selectedChecker);
				//send data
			}
		}

		canMove = true;
		if (currentPlayer[selectedChecker].position.x + 20 > 80 || currentPlayer[selectedChecker].position.z + 20 > 80)
		{
			canMove = false;
		}
		if (canMove)
		{
			if (canJumpDirectionTwo(currentPlayer, otherPlayer, currentPlayer[selectedChecker]))
			{
				currentPlayer[selectedChecker].position = glm::vec3(currentPlayer[selectedChecker].position.x + 20, 1.25, currentPlayer[selectedChecker].position.z + 20);
				currentPlayer[selectedChecker].color = colour;
				otherPlayer[findCheckerToDie(TwoJump, currentPlayer, otherPlayer, currentPlayer[selectedChecker])].isALive = false;
				MoveDeadCheckers(otherPlayer);
				RecursiveJumping(currentPlayer, otherPlayer, colour, selectedChecker);
				//send data
			}
		}

		canMove = true;
		if (currentPlayer[selectedChecker].position.x + 20 > 80 || currentPlayer[selectedChecker].position.z - 20 < 10)
		{
			canMove = false;
		}
		if (canMove)
		{
			if (canJumpDirectionThree(currentPlayer, otherPlayer, currentPlayer[selectedChecker]))
			{
				currentPlayer[selectedChecker].position = glm::vec3(currentPlayer[selectedChecker].position.x + 20, 1.25, currentPlayer[selectedChecker].position.z - 20);
				currentPlayer[selectedChecker].color = colour;
				otherPlayer[findCheckerToDie(ThreeJump, currentPlayer, otherPlayer, currentPlayer[selectedChecker])].isALive = false;
				MoveDeadCheckers(otherPlayer);
				RecursiveJumping(currentPlayer, otherPlayer, colour, selectedChecker);
				//send data
			}
		}

		canMove = true;
		if (currentPlayer[selectedChecker].position.x - 20 < 10 || currentPlayer[selectedChecker].position.z - 20 < 10)
		{
			canMove = false;
		}
		if (canMove)
		{
			if (canJumpDirectionFour(currentPlayer, otherPlayer, currentPlayer[selectedChecker]))
			{
				currentPlayer[selectedChecker].position = glm::vec3(currentPlayer[selectedChecker].position.x - 20, 1.25, currentPlayer[selectedChecker].position.z - 20);
				currentPlayer[selectedChecker].color = colour;
				otherPlayer[findCheckerToDie(FourJump, currentPlayer, otherPlayer, currentPlayer[selectedChecker])].isALive = false;
				MoveDeadCheckers(otherPlayer);
				RecursiveJumping(currentPlayer, otherPlayer, colour, selectedChecker);
				//send data
			}
		}
	}
	else
	{
		switch (currentPlayer[selectedChecker].type)
		{
		case RedType:
		{
			bool canMove = true;
			if (currentPlayer[selectedChecker].position.x - 20 < 10 || currentPlayer[selectedChecker].position.z + 20 > 80)
			{
				canMove = false;
			}
			if (canMove)
			{
				if (canJumpDirectionOne(currentPlayer, otherPlayer, currentPlayer[selectedChecker]))
				{
					currentPlayer[selectedChecker].position = glm::vec3(currentPlayer[selectedChecker].position.x - 20, 1.25, currentPlayer[selectedChecker].position.z + 20);
					currentPlayer[selectedChecker].color = colour;
					otherPlayer[findCheckerToDie(OneJump, currentPlayer, otherPlayer, currentPlayer[selectedChecker])].isALive = false;
					MoveDeadCheckers(otherPlayer);
					RecursiveJumping(currentPlayer, otherPlayer, colour, selectedChecker);
					//send data
				}
			}
			canMove = true;
			if (currentPlayer[selectedChecker].position.x + 20 > 80 || currentPlayer[selectedChecker].position.z + 20 > 80)
			{
				canMove = false;
			}
			if (canMove)
			{
				if (canJumpDirectionTwo(currentPlayer, otherPlayer, currentPlayer[selectedChecker]))
				{
					currentPlayer[selectedChecker].position = glm::vec3(currentPlayer[selectedChecker].position.x + 20, 1.25, currentPlayer[selectedChecker].position.z + 20);
					currentPlayer[selectedChecker].color = colour;
					otherPlayer[findCheckerToDie(TwoJump, currentPlayer, otherPlayer, currentPlayer[selectedChecker])].isALive = false;
					MoveDeadCheckers(otherPlayer);
					RecursiveJumping(currentPlayer, otherPlayer, colour, selectedChecker);
					//send data
				}
			}
			break;
		}
		case BlueType:
		{
			bool canMove = true;
			if (currentPlayer[selectedChecker].position.x + 20 > 80 || currentPlayer[selectedChecker].position.z - 20 < 10)
			{
				canMove = false;
			}
			if (canMove)
			{
				if (canJumpDirectionThree(currentPlayer, otherPlayer, currentPlayer[selectedChecker]))
				{
					currentPlayer[selectedChecker].position = glm::vec3(currentPlayer[selectedChecker].position.x + 20, 1.25, currentPlayer[selectedChecker].position.z - 20);
					currentPlayer[selectedChecker].color = colour;
					otherPlayer[findCheckerToDie(ThreeJump, currentPlayer, otherPlayer, currentPlayer[selectedChecker])].isALive = false;
					MoveDeadCheckers(otherPlayer);
					RecursiveJumping(currentPlayer, otherPlayer, colour, selectedChecker);
					//send data
				}
			}
			canMove = true;
			if (currentPlayer[selectedChecker].position.x - 20 < 10 || currentPlayer[selectedChecker].position.z - 20 < 10)
			{
				canMove = false;
			}
			if (canMove)
			{
				if (canJumpDirectionFour(currentPlayer, otherPlayer, currentPlayer[selectedChecker]))
				{
					currentPlayer[selectedChecker].position = glm::vec3(currentPlayer[selectedChecker].position.x - 20, 1.25, currentPlayer[selectedChecker].position.z - 20);
					currentPlayer[selectedChecker].color = colour;
					otherPlayer[findCheckerToDie(FourJump, currentPlayer, otherPlayer, currentPlayer[selectedChecker])].isALive = false;
					MoveDeadCheckers(otherPlayer);
					RecursiveJumping(currentPlayer, otherPlayer, colour, selectedChecker);
					//send data
				}
			}
			break;
		}
		default:
			break;
		}
	}

}

void ServerSideGame::Movement(glm::vec3 pickPosition, Checker currentPlayer[], Checker otherPlayer[], int selectedChecker, State &state, glm::vec4 colour)
{
	if (currentPlayer[selectedChecker].crowned)
	{
		if (pickPosition.x > currentPlayer[selectedChecker].position.x - 5 - 10 &&
			pickPosition.x < currentPlayer[selectedChecker].position.x + 5 - 10 &&
			pickPosition.z > currentPlayer[selectedChecker].position.z - 5 + 10 &&
			pickPosition.z < currentPlayer[selectedChecker].position.z + 5 + 10)
		{
			if (canMoveDirectionOne(currentPlayer, otherPlayer, currentPlayer[selectedChecker], 10))
			{
				currentPlayer[selectedChecker].position = glm::vec3(currentPlayer[selectedChecker].position.x - 10, 1.25, currentPlayer[selectedChecker].position.z + 10);
				currentPlayer[selectedChecker].color = colour;
				SendData();
			}
		}
		if (pickPosition.x > currentPlayer[selectedChecker].position.x - 5 + 10 &&
			pickPosition.x < currentPlayer[selectedChecker].position.x + 5 + 10 &&
			pickPosition.z > currentPlayer[selectedChecker].position.z - 5 + 10 &&
			pickPosition.z < currentPlayer[selectedChecker].position.z + 5 + 10)
		{

			if (canMoveDirectionTwo(currentPlayer, otherPlayer, currentPlayer[selectedChecker], 10))
			{
				currentPlayer[selectedChecker].position = glm::vec3(currentPlayer[selectedChecker].position.x + 10, 1.25, currentPlayer[selectedChecker].position.z + 10);
				currentPlayer[selectedChecker].color = colour;
				SendData();
			}
		}
		if (pickPosition.x > currentPlayer[selectedChecker].position.x - 5 + 10 &&
			pickPosition.x < currentPlayer[selectedChecker].position.x + 5 + 10 &&
			pickPosition.z > currentPlayer[selectedChecker].position.z - 5 - 10 &&
			pickPosition.z < currentPlayer[selectedChecker].position.z + 5 - 10)
		{
			if (canMoveDirectionThree(currentPlayer, otherPlayer, currentPlayer[selectedChecker], 10))
			{
				currentPlayer[selectedChecker].position = glm::vec3(currentPlayer[selectedChecker].position.x + 10, 1.25, currentPlayer[selectedChecker].position.z - 10);
				currentPlayer[selectedChecker].color = colour;
				SendData();
			}
		}
		if (pickPosition.x > currentPlayer[selectedChecker].position.x - 5 - 10 &&
			pickPosition.x < currentPlayer[selectedChecker].position.x + 5 - 10 &&
			pickPosition.z > currentPlayer[selectedChecker].position.z - 5 - 10 &&
			pickPosition.z < currentPlayer[selectedChecker].position.z + 5 - 10)
		{
			if (canMoveDirectionFour(currentPlayer, otherPlayer, currentPlayer[selectedChecker], 10))
			{
				currentPlayer[selectedChecker].position = glm::vec3(currentPlayer[selectedChecker].position.x - 10, 1.25, currentPlayer[selectedChecker].position.z - 10);
				currentPlayer[selectedChecker].color = colour;
				SendData();
			}
		}
	}
	else
	{
		switch (currentPlayer[selectedChecker].type)
		{
		case RedType:
		{
			if (pickPosition.x > currentPlayer[selectedChecker].position.x - 5 - 10 &&
				pickPosition.x < currentPlayer[selectedChecker].position.x + 5 - 10 &&
				pickPosition.z > currentPlayer[selectedChecker].position.z - 5 + 10 &&
				pickPosition.z < currentPlayer[selectedChecker].position.z + 5 + 10)
			{
				if (canMoveDirectionOne(currentPlayer, otherPlayer, currentPlayer[selectedChecker], 10))
				{
					currentPlayer[selectedChecker].position = glm::vec3(currentPlayer[selectedChecker].position.x - 10, 1.25, currentPlayer[selectedChecker].position.z + 10);
					currentPlayer[selectedChecker].color = colour;
					SendData();
				}
			}
			if (pickPosition.x > currentPlayer[selectedChecker].position.x - 5 + 10 &&
				pickPosition.x < currentPlayer[selectedChecker].position.x + 5 + 10 &&
				pickPosition.z > currentPlayer[selectedChecker].position.z - 5 + 10 &&
				pickPosition.z < currentPlayer[selectedChecker].position.z + 5 + 10)
			{

				if (canMoveDirectionTwo(currentPlayer, otherPlayer, currentPlayer[selectedChecker], 10))
				{
					currentPlayer[selectedChecker].position = glm::vec3(currentPlayer[selectedChecker].position.x + 10, 1.25, currentPlayer[selectedChecker].position.z + 10);
					currentPlayer[selectedChecker].color = colour;
					SendData();
				}
			}
			break;
		}
		case BlueType:
		{
			if (pickPosition.x > currentPlayer[selectedChecker].position.x - 5 + 10 &&
				pickPosition.x < currentPlayer[selectedChecker].position.x + 5 + 10 &&
				pickPosition.z > currentPlayer[selectedChecker].position.z - 5 - 10 &&
				pickPosition.z < currentPlayer[selectedChecker].position.z + 5 - 10)
			{
				if (canMoveDirectionThree(currentPlayer, otherPlayer, currentPlayer[selectedChecker], 10))
				{
					currentPlayer[selectedChecker].position = glm::vec3(currentPlayer[selectedChecker].position.x + 10, 1.25, currentPlayer[selectedChecker].position.z - 10);
					currentPlayer[selectedChecker].color = colour;
					SendData();
				}
			}
			if (pickPosition.x > currentPlayer[selectedChecker].position.x - 5 - 10 &&
				pickPosition.x < currentPlayer[selectedChecker].position.x + 5 - 10 &&
				pickPosition.z > currentPlayer[selectedChecker].position.z - 5 - 10 &&
				pickPosition.z < currentPlayer[selectedChecker].position.z + 5 - 10)
			{
				if (canMoveDirectionFour(currentPlayer, otherPlayer, currentPlayer[selectedChecker], 10))
				{
					currentPlayer[selectedChecker].position = glm::vec3(currentPlayer[selectedChecker].position.x - 10, 1.25, currentPlayer[selectedChecker].position.z - 10);
					currentPlayer[selectedChecker].color = colour;
					SendData();
				}
			}
			break;
		}
		default:
			break;
		}
	}


}

void ServerSideGame::Jumping(glm::vec3 pickPosition, Checker currentPlayer[], Checker otherPlayer[], int selectedChecker, State &state, glm::vec4 colour)
{
	if (currentPlayer[selectedChecker].crowned)
	{
		if (pickPosition.x > currentPlayer[selectedChecker].position.x - 5 - 20 &&
			pickPosition.x < currentPlayer[selectedChecker].position.x + 5 - 20 &&
			pickPosition.z > currentPlayer[selectedChecker].position.z - 5 + 20 &&
			pickPosition.z < currentPlayer[selectedChecker].position.z + 5 + 20)
		{
			if (canJumpDirectionOne(currentPlayer, otherPlayer, currentPlayer[selectedChecker]))
			{
				currentPlayer[selectedChecker].position = glm::vec3(currentPlayer[selectedChecker].position.x - 20, 1.25, currentPlayer[selectedChecker].position.z + 20);
				currentPlayer[selectedChecker].color = colour;
				otherPlayer[findCheckerToDie(OneJump, currentPlayer, otherPlayer, currentPlayer[selectedChecker])].isALive = false;
				MoveDeadCheckers(otherPlayer);
				RecursiveJumping(currentPlayer, otherPlayer, colour, selectedChecker);
				SendData();
			}
		}

		if (pickPosition.x > currentPlayer[selectedChecker].position.x - 5 + 20 &&
			pickPosition.x < currentPlayer[selectedChecker].position.x + 5 + 20 &&
			pickPosition.z > currentPlayer[selectedChecker].position.z - 5 + 20 &&
			pickPosition.z < currentPlayer[selectedChecker].position.z + 5 + 20)
		{
			if (canJumpDirectionTwo(currentPlayer, otherPlayer, currentPlayer[selectedChecker]))
			{
				currentPlayer[selectedChecker].position = glm::vec3(currentPlayer[selectedChecker].position.x + 20, 1.25, currentPlayer[selectedChecker].position.z + 20);
				currentPlayer[selectedChecker].color = colour;
				otherPlayer[findCheckerToDie(TwoJump, currentPlayer, otherPlayer, currentPlayer[selectedChecker])].isALive = false;
				MoveDeadCheckers(otherPlayer);
				RecursiveJumping(currentPlayer, otherPlayer, colour, selectedChecker);
				SendData();
			}
		}

		if (pickPosition.x > currentPlayer[selectedChecker].position.x - 5 + 20 &&
			pickPosition.x < currentPlayer[selectedChecker].position.x + 5 + 20 &&
			pickPosition.z > currentPlayer[selectedChecker].position.z - 5 - 20 &&
			pickPosition.z < currentPlayer[selectedChecker].position.z + 5 - 20)
		{
			if (canJumpDirectionThree(currentPlayer, otherPlayer, currentPlayer[selectedChecker]))
			{
				currentPlayer[selectedChecker].position = glm::vec3(currentPlayer[selectedChecker].position.x + 20, 1.25, currentPlayer[selectedChecker].position.z - 20);
				currentPlayer[selectedChecker].color = colour;
				otherPlayer[findCheckerToDie(ThreeJump, currentPlayer, otherPlayer, currentPlayer[selectedChecker])].isALive = false;
				MoveDeadCheckers(otherPlayer);
				RecursiveJumping(currentPlayer, otherPlayer, colour, selectedChecker);
				SendData();
			}
		}
		if (pickPosition.x > currentPlayer[selectedChecker].position.x - 5 - 20 &&
			pickPosition.x < currentPlayer[selectedChecker].position.x + 5 - 20 &&
			pickPosition.z > currentPlayer[selectedChecker].position.z - 5 - 20 &&
			pickPosition.z < currentPlayer[selectedChecker].position.z + 5 - 20)
		{
			if (canJumpDirectionFour(currentPlayer, otherPlayer, currentPlayer[selectedChecker]))
			{
				currentPlayer[selectedChecker].position = glm::vec3(currentPlayer[selectedChecker].position.x - 20, 1.25, currentPlayer[selectedChecker].position.z - 20);
				currentPlayer[selectedChecker].color = colour;
				otherPlayer[findCheckerToDie(FourJump, currentPlayer, otherPlayer, currentPlayer[selectedChecker])].isALive = false;
				MoveDeadCheckers(otherPlayer);
				RecursiveJumping(currentPlayer, otherPlayer, colour, selectedChecker);
				SendData();
			}
		}
	}
	else
	{
		switch (currentPlayer[selectedChecker].type)
		{
		case RedType:
		{
			if (pickPosition.x > currentPlayer[selectedChecker].position.x - 5 - 20 &&
				pickPosition.x < currentPlayer[selectedChecker].position.x + 5 - 20 &&
				pickPosition.z > currentPlayer[selectedChecker].position.z - 5 + 20 &&
				pickPosition.z < currentPlayer[selectedChecker].position.z + 5 + 20)
			{
				if (canJumpDirectionOne(currentPlayer, otherPlayer, currentPlayer[selectedChecker]))
				{
					currentPlayer[selectedChecker].position = glm::vec3(currentPlayer[selectedChecker].position.x - 20, 1.25, currentPlayer[selectedChecker].position.z + 20);
					currentPlayer[selectedChecker].color = colour;
					otherPlayer[findCheckerToDie(OneJump, currentPlayer, otherPlayer, currentPlayer[selectedChecker])].isALive = false;
					MoveDeadCheckers(otherPlayer);
					RecursiveJumping(currentPlayer, otherPlayer, colour, selectedChecker);
					SendData();
				}
			}
			if (pickPosition.x > currentPlayer[selectedChecker].position.x - 5 + 20 &&
				pickPosition.x < currentPlayer[selectedChecker].position.x + 5 + 20 &&
				pickPosition.z > currentPlayer[selectedChecker].position.z - 5 + 20 &&
				pickPosition.z < currentPlayer[selectedChecker].position.z + 5 + 20)
			{
				if (canJumpDirectionTwo(currentPlayer, otherPlayer, currentPlayer[selectedChecker]))
				{
					currentPlayer[selectedChecker].position = glm::vec3(currentPlayer[selectedChecker].position.x + 20, 1.25, currentPlayer[selectedChecker].position.z + 20);
					currentPlayer[selectedChecker].color = colour;
					otherPlayer[findCheckerToDie(TwoJump, currentPlayer, otherPlayer, currentPlayer[selectedChecker])].isALive = false;
					MoveDeadCheckers(otherPlayer);
					RecursiveJumping(currentPlayer, otherPlayer, colour, selectedChecker);
					SendData();
				}
			}
			break;
		}
		case BlueType:
		{
			if (pickPosition.x > currentPlayer[selectedChecker].position.x - 5 + 20 &&
				pickPosition.x < currentPlayer[selectedChecker].position.x + 5 + 20 &&
				pickPosition.z > currentPlayer[selectedChecker].position.z - 5 - 20 &&
				pickPosition.z < currentPlayer[selectedChecker].position.z + 5 - 20)
			{
				if (canJumpDirectionThree(currentPlayer, otherPlayer, currentPlayer[selectedChecker]))
				{
					currentPlayer[selectedChecker].position = glm::vec3(currentPlayer[selectedChecker].position.x + 20, 1.25, currentPlayer[selectedChecker].position.z - 20);
					currentPlayer[selectedChecker].color = colour;
					otherPlayer[findCheckerToDie(ThreeJump, currentPlayer, otherPlayer, currentPlayer[selectedChecker])].isALive = false;
					MoveDeadCheckers(otherPlayer);
					RecursiveJumping(currentPlayer, otherPlayer, colour, selectedChecker);
					SendData();
				}
			}
			if (pickPosition.x > currentPlayer[selectedChecker].position.x - 5 - 20 &&
				pickPosition.x < currentPlayer[selectedChecker].position.x + 5 - 20 &&
				pickPosition.z > currentPlayer[selectedChecker].position.z - 5 - 20 &&
				pickPosition.z < currentPlayer[selectedChecker].position.z + 5 - 20)
			{
				if (canJumpDirectionFour(currentPlayer, otherPlayer, currentPlayer[selectedChecker]))
				{
					currentPlayer[selectedChecker].position = glm::vec3(currentPlayer[selectedChecker].position.x - 20, 1.25, currentPlayer[selectedChecker].position.z - 20);
					currentPlayer[selectedChecker].color = colour;
					otherPlayer[findCheckerToDie(FourJump, currentPlayer, otherPlayer, currentPlayer[selectedChecker])].isALive = false;
					MoveDeadCheckers(otherPlayer);
					RecursiveJumping(currentPlayer, otherPlayer, colour, selectedChecker);
					SendData();
				}
			}
			break;
		}
		default:
			break;
		}
	}


}

int ServerSideGame::findCheckerToDie(Directions dir, Checker currentPlayer[], Checker otherPlayer[], const Checker &selected)
{
	switch (dir)
	{
	case OneJump:
		for (int j = 0; j < 12; j++)
		{
			if ((otherPlayer[j].position.x > selected.position.x - 5 + 10 &&
				otherPlayer[j].position.x < selected.position.x + 5 + 10 &&
				otherPlayer[j].position.z > selected.position.z - 5 - 10 &&
				otherPlayer[j].position.z < selected.position.z + 5 - 10))
			{
				return j;
			}
		}
		break;
	case TwoJump:
		for (int j = 0; j < 12; j++)
		{
			if ((otherPlayer[j].position.x > selected.position.x - 5 - 10 &&
				otherPlayer[j].position.x < selected.position.x + 5 - 10 &&
				otherPlayer[j].position.z > selected.position.z - 5 - 10 &&
				otherPlayer[j].position.z < selected.position.z + 5 - 10))
			{
				return j;
			}
		}
		break;
	case ThreeJump:
		for (int j = 0; j < 12; j++)
		{
			if ((otherPlayer[j].position.x > selected.position.x - 5 - 10 &&
				otherPlayer[j].position.x < selected.position.x + 5 - 10 &&
				otherPlayer[j].position.z > selected.position.z - 5 + 10 &&
				otherPlayer[j].position.z < selected.position.z + 5 + 10))
			{
				return j;
			}

		}
		break;
	case FourJump:
		for (int j = 0; j < 12; j++)
		{
			if ((otherPlayer[j].position.x > selected.position.x - 5 + 10 &&
				otherPlayer[j].position.x < selected.position.x + 5 + 10 &&
				otherPlayer[j].position.z > selected.position.z - 5 + 10 &&
				otherPlayer[j].position.z < selected.position.z + 5 + 10))
			{
				return j;
			}
		}
		break;
	default:
		break;
	}
}

void ServerSideGame::SendData()
{
	DidBlueCrown(Blue);
	DidRedCrown(Red);
	serverTurnSwap = !serverTurnSwap;
	state = Moved;
	for (int i = 0; i < 2; i++)
	{
		RakNet::BitStream bsOut;
		bsOut.Write((RakNet::MessageID)GameMessages::ID_SERVER_DECISION);
		bsOut.Write(id); // game id
		bsOut.Write(players[i].uiConnectionID); // client id
		(i == serverTurnSwap) ? bsOut.Write(turn) : bsOut.Write(!turn);	// trun
		bsOut.Write((player == Player_1) ? Player_2 : Player_1); // playerturn
		bsOut.Write(state); // playerturn
		for (int j = 0; j < 12; j++)
		{
			bsOut.Write(Red[j].position.x);
			bsOut.Write(Red[j].position.y);
			bsOut.Write(Red[j].position.z);
			bsOut.Write(Red[j].isALive);
			bsOut.Write(Red[j].crowned);
			bsOut.Write(Red[j].color);

			bsOut.Write(Blue[j].position.x);
			bsOut.Write(Blue[j].position.y);
			bsOut.Write(Blue[j].position.z);
			bsOut.Write(Blue[j].isALive);
			bsOut.Write(Blue[j].crowned);
			bsOut.Write(Blue[j].color);
		}// checker positions
		m_pPeerInterface->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, players[i].sysAddress, false);
	}
}

bool ServerSideGame::DidRedCrown(Checker currentPlayer[])
{
	for (int i = 0; i < 12; i++)
	{
		if (currentPlayer[i].position.z == 80)
		{
			currentPlayer[i].crowned = true;
			return true;
		}
	}
}
bool ServerSideGame::DidBlueCrown(Checker currentPlayer[])
{
	for (int i = 0; i < 12; i++)
	{
		if (currentPlayer[i].position.z == 10)
		{
			currentPlayer[i].crowned = true;
			return true;
		}
	}
}
