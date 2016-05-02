#pragma once

#include <vector>
#include <glm\glm.hpp>

#include "GameObject.h"
#include "BaseApplication.h"
#include <RakPeerInterface.h>
#include <RakString.h>

namespace RakNet {
	class RakPeerInterface;
}

class Camera;

class BasicNetworkingApplication : public BaseApplication {
public:
	BasicNetworkingApplication();
	virtual ~BasicNetworkingApplication();

	virtual bool startup();
	virtual void shutdown();

	virtual bool update(float deltaTime);

	virtual void draw();

	// Initialize the connection
	void handleNetworkConnection();
	void initialiseClientConnection();
	void handleNetworkMessages();
	void readObjectDataFromServer(RakNet::BitStream& bsIn);
	void createGameObject();
	void waitInGameLobby();
	void moveClientObject(float deltaTime);
	void sendUpdatedObjectPositionToServer(GameObject&);

	void startGame(RakNet::BitStream& bsIn);

	RakNet::RakPeerInterface* m_pPeerInterface;
	const char* IP = "10.19.20.32";
	const unsigned short PORT = 5456;
	unsigned int m_uiClientId;

	Camera*		m_camera;

	glm::vec4 m_myColour;
	std::vector<GameObject> m_gameObjects;

	int m_uiclientObjectIndex;

private:

};