#pragma once

enum Type
{
	Board,
	PlayerOneChecker,
	PlayerTwoChecker
};


struct GameObject {
	unsigned int uiOwnerClientID;
	unsigned int uiObjectID;

	float fRedColour;
	float fGreenColour;
	float fBlueColour;

	Type type;

	float fXPos;
	float fZPos;
};