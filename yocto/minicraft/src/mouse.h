#pragma once

#include "engine/utils/types_3d.h"

class Mouse
{
public:
	static void changeMode()
	{
		Mouse::fpv = !Mouse::fpv;
		if (fpv)
			glutSetCursor(GLUT_CURSOR_NONE);
		else
			glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);

		Mouse::position = Mouse::newPosition;
	}

	static void goToNewPos()
	{
		//Warp mouse in loop (10px margin)
		const YRenderer& renderer = *YEngine::getInstance()->Renderer;
		YVec3<int> mousePos = Mouse::getMousePosition();
		if (mousePos.X > renderer.ScreenWidth * 4/5)
			Mouse::resetMousePos(mousePos.X - renderer.ScreenWidth * 3/5, mousePos.Y);
		else if (mousePos.X < renderer.ScreenWidth * 1/5)
			Mouse::resetMousePos(mousePos.X + renderer.ScreenWidth * 3/5, mousePos.Y);
		if (mousePos.Y > renderer.ScreenHeight * 4/5)
			Mouse::resetMousePos(mousePos.X, mousePos.Y - renderer.ScreenHeight * 3/5);
		else if (mousePos.Y < renderer.ScreenHeight * 1/5)
			Mouse::resetMousePos(mousePos.X, mousePos.Y + renderer.ScreenHeight * 3 / 5);

		Mouse::position = Mouse::newPosition;
	}

	static void setNewPosition(YVec3<int> newPosition)
	{
		Mouse::newPosition.X = newPosition.X;
		Mouse::newPosition.Y = newPosition.Y;

		YVec3<int> deltaInt = Mouse::newPosition - position;
		deltaPosition = { (float)deltaInt.X, (float)deltaInt.Y , 0.0f };
	}

	static void resetMousePos(int x, int y)
	{
		glutWarpPointer(x , y);

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

	static bool fpv;
};

YVec3<int> Mouse::position{};
YVec3<int> Mouse::newPosition{};
YVec3<float> Mouse::deltaPosition{};
float Mouse::sensitivity = 0.2f;
bool Mouse::fpv = false;