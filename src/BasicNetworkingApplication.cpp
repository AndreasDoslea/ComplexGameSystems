#include "BasicNetworkingApplication.h"
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include <RakPeerInterface.h>
#include <RakString.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>
#include "GameMessages.h"
#include "Gizmos.h"
#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <glm\vec3.hpp>
#include <glm\vec4.hpp>

#include "ClientSideGame.h"

BasicNetworkingApplication::BasicNetworkingApplication() {

}

BasicNetworkingApplication::~BasicNetworkingApplication() {

}

bool BasicNetworkingApplication::startup() {
	// setup the basic window
	createWindow("Client Application", 1280, 720);

	// start the gizmo system that can draw basic shapes
	Gizmos::create();

	// create a camera
	m_camera = new Camera(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 15000.0f);
	m_camera->setLookAtFrom(glm::vec3(10, 10, 10), glm::vec3(0));

	handleNetworkConnection();

	m_myColour = glm::vec4((rand() % 10 + 1) / 10.0f, (rand() % 10 + 1) / 10.0f, (rand() % 10 + 1) / 10.0f, 1.0f);

	return true;
}

void BasicNetworkingApplication::shutdown()
{
	// delete our camera and cleanup gizmos
	delete m_camera;
	Gizmos::destroy();

	// destroy our window properly
	destroyWindow();
}

bool BasicNetworkingApplication::update(float deltaTime) {
	// close the application if the window closes
	if (glfwWindowShouldClose(m_window) ||
		glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		return false;

	// clear the gizmos out for this frame
	Gizmos::clear();

	// update the camera's movement
	m_camera->update(deltaTime);

	handleNetworkMessages();

	moveClientObject(deltaTime);

	if (m_gameObjects.size() > 0)
		std::cout << "My position" << m_gameObjects[m_uiclientObjectIndex].fZPos << std::endl;

	for (int i = 0; i < m_gameObjects.size(); i++)
	{
		GameObject& obj = m_gameObjects[i];
		Gizmos::addSphere(glm::vec3(obj.fXPos, 2, obj.fZPos),
			2, 32, 32, glm::vec4(obj.fRedColour, obj.fGreenColour,
				obj.fBlueColour, 1), nullptr);
	}

	// ...for now let's add a grid to the gizmos
	for (int i = 0; i < 21; ++i) {
		Gizmos::addHermiteSpline(glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10), glm::vec3(0, 50, 0), glm::vec3(0, -50, 0), 200, glm::vec4(0, 0, 0, 1));
		Gizmos::addHermiteSpline(glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i), glm::vec3(0, -50, 0), glm::vec3(0, 50, 0), 200, glm::vec4(0, 0, 0, 1));
	}

	Gizmos::addLine(glm::vec3(-10, 0, -10), glm::vec3(-10, 0, 10), glm::vec4(0, 0, 0, 1));
	Gizmos::addLine(glm::vec3(-10, 0, -10), glm::vec3(10, 0, -10), glm::vec4(0, 0, 0, 1));
	Gizmos::addLine(glm::vec3(10, 0, 10), glm::vec3(-10, 0, 10), glm::vec4(0, 0, 0, 1));
	Gizmos::addLine(glm::vec3(10, 0, 10), glm::vec3(10, 0, -10), glm::vec4(0, 0, 0, 1));

	return true;
}

void BasicNetworkingApplication::draw()
{
	glClearColor(1, 1, 1, 1); 

	// clear the screen for this frame
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	// display the 3D gizmos
	Gizmos::draw(m_camera->getProjectionView());

	// get a orthographic projection matrix and draw 2D gizmos
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	glm::mat4 guiMatrix = glm::ortho<float>(0, 0, (float)width, (float)height);

	Gizmos::draw2D(m_camera->getProjectionView());
}

void BasicNetworkingApplication::handleNetworkConnection()
{
	//Initialize the Raknet peer interface first
	m_pPeerInterface = RakNet::RakPeerInterface::GetInstance();
	initialiseClientConnection();
}

void BasicNetworkingApplication::initialiseClientConnection()
{
	//Create a socket descriptor to describe this connection
	//No data needed, as we will be connecting to a server
	RakNet::SocketDescriptor sd;
	//Now call startup - max of 1 connections (to the server)
	m_pPeerInterface->Startup(1, &sd, 1);
	std::cout << "Connectiong to server at: " << IP << std::endl;
	//Now call connect to attempt to connect to the given server
	RakNet::ConnectionAttemptResult res = m_pPeerInterface->Connect(IP, PORT, nullptr, 0);
	//Finally, check to see if we connected, and if not, throw a error
	if (res != RakNet::CONNECTION_ATTEMPT_STARTED)
	{
		std::cout << "Unable to start connection, Error number: " << res << std::endl;
	}
}

void BasicNetworkingApplication::handleNetworkMessages()
{
	RakNet::Packet* packet;
	for (packet = m_pPeerInterface->Receive(); packet;
	m_pPeerInterface->DeallocatePacket(packet),
		packet = m_pPeerInterface->Receive())
	{
		switch (packet->data[0])
		{
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			std::cout << "Another client has disconnected.\n";
			break;
		case ID_REMOTE_CONNECTION_LOST:
			std::cout << "Another client has lost the connection.\n";
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			std::cout << "Another client has connected.\n";
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
			std::cout << "Our connection request has been accepted.\n";
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			std::cout << "The server is full.\n";
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			std::cout << "We have been disconnected.\n";
			break;
		case ID_CONNECTION_LOST:
			std::cout << "Connection lost.\n";
			break;
		case ID_SERVER_FULL_OBJECT_DATA:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			readObjectDataFromServer(bsIn);
			break;
		}
		case ID_SERVER_CLIENT_ID:
		{
			//make this to waiting in the game lobby 
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(m_uiClientId);
			std::cout << "Server has given us an id of: " << m_uiClientId << std::endl;
			waitInGameLobby();
			//createGameObject();
			break;
		}
		case ID_SET_UP_GAME:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			startGame(bsIn);
			break;
		}
		case ID_SERVER_TEXT_MESSAGE:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			RakNet::RakString str;
			bsIn.Read(str);
			std::cout << str.C_String() << std::endl;
			break;
		}
		default:
			std::cout << "Received a message with a unknown id: " << packet->data[0];
			break;
		}
	}
}

void BasicNetworkingApplication::readObjectDataFromServer(RakNet::BitStream& bsIn)
{
	//Create a temp object that we will pull all the object data into
	GameObject tempGameObject;

	//Read in the object data
	bsIn.Read(tempGameObject.fXPos);
	bsIn.Read(tempGameObject.fZPos);
	bsIn.Read(tempGameObject.fRedColour);
	bsIn.Read(tempGameObject.fGreenColour);
	bsIn.Read(tempGameObject.fBlueColour);
	bsIn.Read(tempGameObject.uiOwnerClientID);
	bsIn.Read(tempGameObject.uiObjectID);

	//Check to see whether or not this object is already stored in our local object list
	bool bFound = false;
	for (int i = 0; i < m_gameObjects.size(); i++)
	{
		if (m_gameObjects[i].uiObjectID == tempGameObject.uiObjectID)
		{
			bFound = true;
			//Update the game object
			GameObject& obj = m_gameObjects[i];
			obj.fXPos = tempGameObject.fXPos;
			obj.fZPos = tempGameObject.fZPos;
			obj.fRedColour = tempGameObject.fRedColour;
			obj.fGreenColour = tempGameObject.fGreenColour;
			obj.fBlueColour = tempGameObject.fBlueColour;
			obj.uiOwnerClientID = tempGameObject.uiOwnerClientID;
		}
	}
	//If we didn't find it, then it is a new object - add it to our object list
	if (!bFound)
	{
		m_gameObjects.push_back(tempGameObject);
		if (tempGameObject.uiOwnerClientID == m_uiClientId)
		{
			m_uiclientObjectIndex = m_gameObjects.size() - 1;
		}
	}
}

void BasicNetworkingApplication::createGameObject()
{
	//Tell the server we want to create a new game object that will represent us
	RakNet::BitStream bsOut;
	GameObject tempGameObject;
	tempGameObject.fXPos = 5.0f;
	tempGameObject.fZPos = 10.0f;
	tempGameObject.fRedColour = m_myColour.r;
	tempGameObject.fGreenColour = m_myColour.g;
	tempGameObject.fBlueColour = m_myColour.b;
	//Ensure that the write order is the same as the read order on the server!
	bsOut.Write((RakNet::MessageID)GameMessages::ID_CLIENT_CREATE_OBJECT);
	bsOut.Write(tempGameObject.fXPos);
	bsOut.Write(tempGameObject.fZPos);
	bsOut.Write(tempGameObject.fRedColour);
	bsOut.Write(tempGameObject.fGreenColour);
	bsOut.Write(tempGameObject.fBlueColour);
	m_pPeerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
		RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void BasicNetworkingApplication::waitInGameLobby()
{
	//Tell the server we want to create a new game object that will represent us
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)GameMessages::ID_WAIT_IN_LOBBY);
	bsOut.Write(m_uiClientId);
	m_pPeerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
		RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void BasicNetworkingApplication::moveClientObject(float deltaTime)
{
	//We don't have a valid client ID, so we we have no game object!
	if (m_uiClientId == 0) return;
	//No game objects sent to us, so we don't know who we are yet
	if (m_gameObjects.size() == 0) return;
	bool bUpdatedObjectPosition = false;
	GameObject& myClientObject = m_gameObjects[m_uiclientObjectIndex];
	if (glfwGetKey(m_window, GLFW_KEY_UP))
	{
		myClientObject.fZPos += 2 * deltaTime;
		bUpdatedObjectPosition = true;
	}
	if (glfwGetKey(m_window, GLFW_KEY_DOWN))
	{
		myClientObject.fZPos -= 2 * deltaTime;
		bUpdatedObjectPosition = true;
	}
	if (bUpdatedObjectPosition == true)
	{
		sendUpdatedObjectPositionToServer(myClientObject);
	}
}

void BasicNetworkingApplication::sendUpdatedObjectPositionToServer(GameObject& a_gameObject)
{
	//Tell the server we want to create a new game object that will represent us
	RakNet::BitStream bsOut;
	//Ensure that the write order is the same as the read order on the server!
	bsOut.Write((RakNet::MessageID)GameMessages::ID_CLIENT_UPDATE_OBJECT_POSITION);
	bsOut.Write(a_gameObject.fXPos);
	bsOut.Write(a_gameObject.fZPos);
	m_pPeerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
		RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}
void BasicNetworkingApplication::startGame(RakNet::BitStream& bsIn)
{
	shutdown();
	ClientSideGame* Game = new ClientSideGame(bsIn, m_pPeerInterface);
	if (Game->startup())
		Game->run();
	Game->shutdown();
}