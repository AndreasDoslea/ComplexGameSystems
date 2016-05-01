#pragma once


//#include "gl_core_4_4.h"
//
//#include <GLFW/glfw3.h>
//#include <glm/glm.hpp>
//#include <glm/ext.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>

enum Directions {
	One, Two, Three, Four,
	OneJump, TwoJump, ThreeJump, FourJump,
	None
};

enum Type { RedType, BlueType };

class Checker {
private:

public:

	glm::vec3 position;
	glm::vec4 color;
	bool isALive = true;
	bool crowned = false;
	bool canJump = false;
	std::vector<Directions> directions;
	int type;


};