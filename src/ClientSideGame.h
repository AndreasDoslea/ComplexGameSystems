#pragma once
#include "Checker.h"
#include "Gizmos.h"
#include "Camera.h"
#include "BasicNetworkingApplication.h"

class ClientSideGame : public BasicNetworkingApplication {
public:
	ClientSideGame() {}
	ClientSideGame(Camera &m_camera, RakNet::BitStream& bsIn);
	void Start(Camera &camera);


private:

	glm::vec3	m_pickPosition;
	int selectedChecker;

	int id;

	unsigned int recievedId;

	bool turn;

	void Update(float deltatime, GLFWwindow * m_window);
	void DrawGameBoard();
	void Draw();

	std::vector<Directions> CheckMoveableDirections(Checker currentPlayer[], Checker otherPlayer[], const Checker &selected);
	bool DidRedCrown(Checker currentPlayer[]);
	bool DidBlueCrown(Checker currentPlayer[]);
	bool IsClickingChecker(glm::vec3 pickposition, Checker checker[], glm::vec4 selectedColor);

	enum Player { Player_1, Player_2 };
	enum State { Clicked, Moved, Empty };

	Player player = Player_1;
	State state = Empty;

	Checker Red[12];
	Checker Blue[12];

};