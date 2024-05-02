#pragma once

#include "engine/utils/types_3d.h"

class Mouse
{
public:
	static void setNewPosition(YVec3<int> newPosition)
	{
		newPosition.X = newPosition.X;
		newPosition.Y = newPosition.Y;

		YVec3<int> deltaInt = newPosition - position;
		deltaPosition = { (float)deltaInt.X, (float)deltaInt.Y , 0.0f };
	}

	static void resetMousePos(int x, int y)
	{
		glutWarpPointer(x , x);

		position = getMousePosition();
		newPosition = position;
		deltaPosition = {};
	}

	static YVec3<int> getMousePosition()
	{
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		RECT windowRect;
		GetWindowRect(GetActiveWindow(), &windowRect);
		return { cursorPos.x - windowRect.left - 7, cursorPos.y - windowRect.top - 30, 0 };
	}

	static float sensitivity;
	static YVec3<float> deltaPosition;
	static YVec3<int> position;
	static YVec3<int> newPosition;
};

YVec3<int> Mouse::position{};
YVec3<int> Mouse::newPosition{};
YVec3<float> Mouse::deltaPosition{};
float Mouse::sensitivity = 2.0f;