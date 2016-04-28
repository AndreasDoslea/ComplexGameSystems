#include "ClientSideGame.h"

#include <BitStream.h>

#include "gl_core_4_4.h"

#include <GLFW/glfw3.h>

ClientSideGame::ClientSideGame(Camera &camera, RakNet::BitStream& bsIn)
{
	printf("gfgghjgh");
	bsIn.Read(id); // game id
	bsIn.Read(recievedId); // client id
	bsIn.Read(turn);	// trun
	bsIn.Read(player); // playerturn
	for (int j = 0; j > 12; j++)
	{
		bsIn.Read(Red[j].position.x);
		bsIn.Read(Red[j].position.y);
		bsIn.Read(Red[j].position.z);
		bsIn.Read(Red[j].isALive);
		bsIn.Read(Red[j].crowned);

		bsIn.Read(Blue[j].position.x);
		bsIn.Read(Blue[j].position.y);
		bsIn.Read(Blue[j].position.z);
		bsIn.Read(Blue[j].isALive);
		bsIn.Read(Blue[j].crowned);
	}// checker positions
	Start(camera);
}

void ClientSideGame::Start(Camera &camera)
{

	for (int i = 0; i < 12; i++)
	{
		Red[i].color = glm::vec4(1, 0, 0, 1);
		Blue[i].color = glm::vec4(0, 0, 1, 1);
	}

	camera.setLookAtFrom(glm::vec3(10, 50, 10), glm::vec3(50, 0, 50));


}

void ClientSideGame::Update(float deltaTime, GLFWwindow * m_window)
{

	// recieve the data 
	// do the action with shit 
	//tell who can move now

	// update the camera's movement


	//mouse picking
	if (glfwGetMouseButton(m_window, 0) == GLFW_PRESS) {
		double x = 0, y = 0;
		glfwGetCursorPos(m_window, &x, &y);

		// plane represents the ground, with a normal of (0,1,0) and a distance of 0 from (0,0,0)
		glm::vec4 plane(0, 1, 0, 0);
		m_pickPosition = m_camera->pickAgainstPlane((float)x, (float)y, plane);
	}

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
	DidBlueCrown(Blue);
	DidRedCrown(Red);
	if (m_pickPosition.x > 85)
	{
		m_pickPosition = glm::vec3(200);
	}
	if (m_pickPosition.z > 85)
	{
		m_pickPosition = glm::vec3(200);
	}
	if (m_pickPosition.x < 5)
	{
		m_pickPosition = glm::vec3(200);
	}
	if (m_pickPosition.z < 5)
	{
		m_pickPosition = glm::vec3(200);
	}

	if (turn)
	{		
		switch (player)
		{
		case Player_1:
			switch (state)
			{
			case Clicked:

				break;
			case Moved:

				break;
			case Empty:
				if (IsClickingChecker(m_pickPosition, Red, glm::vec4(1, 0.5, 0, 1)))
				{
					state = Clicked;
				}
				break;
			default:
				break;
			}
			break;
		case Player_2:
			switch (state)
			{
			case Clicked:
				// send data 
				break;
			case Moved:

				break;
			case Empty:
				if (IsClickingChecker(m_pickPosition, Blue, glm::vec4(0, 0.5, 1, 1)))
				{
					state = Clicked;
				}
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}

}
void ClientSideGame::DrawGameBoard()
{
	for (int i = 1; i < 9; i++)
	{
		for (int j = 1; j < 9; j++)
		{
		Gizmos::addAABBFilled(glm::vec3(i * 10, 0, j * 10), glm::vec3(5, 1, 5), ((i % 2 == 0) && (j % 2 != 0)) ? glm::vec4(0, 0, 0, 1) : ((i % 2 != 0) && (j % 2 == 0)) ? glm::vec4(0, 0, 0, 1) : glm::vec4(1, 1, 1, 1));
		}
	}
}

void ClientSideGame::Draw()
{
	DrawGameBoard();
	for (int i = 0; i < 12; i++)
	{
		if (Red[i].isALive)
		{
			if (Red[i].crowned)
			{
				Gizmos::addCylinderFilled(Red[i].position, 5, 1, 10, Red[i].color);
			}
			else
			{
				Gizmos::addCylinderFilled(Red[i].position, 5, .25, 10, Red[i].color);
			}
		}
		else
		{
			Red[i].position = glm::vec3(0, i, 10);
			Gizmos::addCylinderFilled(Red[i].position, 5, .25, 10, Red[i].color);
		}
		if (Blue[i].isALive)
		{
			if (Blue[i].crowned)
			{
				Gizmos::addCylinderFilled(Blue[i].position, 5, 1, 10, Blue[i].color);
			}
			else
			{
				Gizmos::addCylinderFilled(Blue[i].position, 5, .25, 10, Blue[i].color);
			}
		}
		else
		{
			Blue[i].position = glm::vec3(90, i, 80);
			Gizmos::addCylinderFilled(Blue[i].position, 5, .25, 10, Blue[i].color);
		}
	}
	
	
	Gizmos::draw(m_camera->getProjectionView());
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

std::vector<Directions> ClientSideGame::CheckMoveableDirections(Checker currentPlayer[], Checker otherPlayer[], const Checker &selected)
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

bool ClientSideGame::DidRedCrown(Checker currentPlayer[])
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
bool ClientSideGame::DidBlueCrown(Checker currentPlayer[])
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

bool ClientSideGame::IsClickingChecker(glm::vec3 pickposition, Checker checker[], glm::vec4 selectedColor)
{

	for (int i = 0; i < 12; i++)
	{
		if (pickposition.x > checker[i].position.x - 5 &&
			pickposition.x < checker[i].position.x + 5 &&
			pickposition.z > checker[i].position.z - 5 &&
			pickposition.z < checker[i].position.z + 5)
		{
			checker[i].color = selectedColor;
			return true;
		}
	}
}