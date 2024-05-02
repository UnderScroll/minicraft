#pragma once

#include "engine/utils/types_3d.h"
#include "engine/render/camera.h"
#include "engine/utils/timer.h"
#include "world.h"
#include "mouse.h"
#include <windows.h>

class MAvatar
{
public:
	YVec3f Position;
	float Speed;
	float RunSpeedAdd;
	float CrouchSpeedSub;

	bool Jump;
	float Height;
	float CurrentHeight;
	float Width;
	YVec3f Direction;
	bool Standing;
	bool InWater;
	bool Crouch;
	bool Run;

	YVec3f LookAt;

	YCamera* Cam;
	MWorld* World;

	YTimer _TimerStanding;

	MAvatar(YCamera* cam, MWorld* world)
	{
		Position = YVec3f(10, 10, 10);
		Height = 1.8f;
		CurrentHeight = Height;
		Width = 0.3f;
		Cam = cam;
		Direction = YVec3f();
		Standing = false;
		Jump = false;
		World = world;
		InWater = false;
		Crouch = false;
		Run = false;
		Speed = 10;
		RunSpeedAdd = 5;
		CrouchSpeedSub = 5;
		LookAt = YVec3f();

		Cam->setPosition(Position + YVec3f(0, 0, Height - 0.2f));
		Cam->setLookAt(LookAt);

		Mouse::changeMode();
	}

	void update(float elapsed)
	{
		handleInput();

		if (elapsed > 1.0f / 60.0f)
			elapsed = 1.0f / 60.0f;

		//Move
		if (Direction.getSqrSize() > 0)
		{
			YVec2f deltaPosXY = YVec2f(Direction.X, Direction.Y);
			deltaPosXY *= Speed + RunSpeedAdd * Run - CrouchSpeedSub * Crouch;
			deltaPosXY *= elapsed;

			float deltaPosZ = Direction.Z * Speed;
			deltaPosZ *= elapsed;

			YVec3f deltaPos = YVec3f(deltaPosXY.X, deltaPosXY.Y, deltaPosZ);
			Position += deltaPos;
			Cam->move(deltaPos);
		}

		if (Mouse::fpv)
		{
			Cam->rotate(-Mouse::deltaPosition.X * Mouse::sensitivity * elapsed);
			Cam->rotateUp(Mouse::deltaPosition.Y * Mouse::sensitivity * elapsed);

			Mouse::goToNewPos();
		}
	}

	bool firstMouse = true;
	int lastX, lastY;


	void handleInput()
	{
		YVec3f forward = Cam->Direction * (GetKeyState('Z') & 0x8000);
		YVec3f backward = Cam->Direction * (GetKeyState('S') & 0x8000);
		YVec3f right = Cam->RightVec * (GetKeyState('D') & 0x8000);
		YVec3f left = Cam->RightVec * (GetKeyState('Q') & 0x8000);

		Direction = forward - backward + left - right;
		Direction.normalize();

		Jump = GetKeyState(VK_SPACE) & 0x8000;
		Crouch = GetKeyState(VK_SHIFT) & 0x8000;
		Run = GetKeyState(VK_LCONTROL) & 0x8000;

		Direction.Z = Jump - Crouch;

		//Update mouse newPosition
		Mouse::setNewPosition(Mouse::getMousePosition());
	}
};