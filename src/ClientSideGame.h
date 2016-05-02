#pragma once
#include "Checker.h"
#include "Gizmos.h"
#include "Camera.h"
#include "BasicNetworkingApplication.h"
#include <RakPeerInterface.h>
#include <RakString.h>

namespace RakNet {
	class RakPeerInterface;
}

class ClientSideGame : public BaseApplication {
public:
	ClientSideGame() {}
	ClientSideGame(RakNet::BitStream& bsIn, RakNet::RakPeerInterface* m_pPeerInterface);

	virtual bool startup();
	virtual void shutdown();

	virtual bool update(float deltaTime);

	virtual void draw();


private:
	RakNet::RakPeerInterface* m_pPeerInterface;

	glm::vec3	m_pickPosition;

	Camera*		m_camera;

	int id;

	unsigned int recievedId;

	bool turn;

	void DrawGameBoard();

	bool IsClickingChecker(glm::vec3 pickposition, Checker checker[], glm::vec4 selectedColor, glm::vec4 OldColour);
	int ClientSideGame::FindClickingChecker(glm::vec3 pickposition, Checker checker[]);

	int selectedChecker = -1;
	enum Player { Player_1, Player_2 };
	enum State { Clicked, Moved, Empty };

	Player player = Player_1;
	State state = Empty;

	Checker Red[12];
	Checker Blue[12];

};