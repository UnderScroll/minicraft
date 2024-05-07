#include "engine/utils/types_3d.h"
#include "engine/render/vbo.h"
#include "engine/engine.h"

#include "shape.h"

#define TWO_PI 3.1415926539 * 2

class Sun
{
public:
	YVec3<float> position;
	YVec3<float> direction;
	YVec3<float> size;
	YVec3<float> sphericalPosition;
	GLuint shader;

	SYSTEMTIME sysTime;
	unsigned int sysTimeSec;
	int offset;
	YColor skyColor;

	Sun()
		:sphericalPosition(YVec3<float>(100.0f, 0.25f * TWO_PI, 0.34f * TWO_PI)),
		shape(CubeShape(CubeShape::ALL_FACES)),
		position(YVec3<float>(0, 0, 0)),
		size(YVec3<float>(10, 10, 10)),
		skyColor(skyColorPalette[0])
	{
	}

	void loadSunShader()
	{
		shader = YEngine::getInstance()->Renderer->createProgram("shaders/sun");
	}

	YVec3<float> getPosition()
	{
		return position + fromSphericalCoordinates(sphericalPosition.X, sphericalPosition.Y, sphericalPosition.Z);
	}

	void updateDirection()
	{
		direction = fromSphericalCoordinates(sphericalPosition.X, sphericalPosition.Y, sphericalPosition.Z).normalize();
	}

	void update()
	{
		if (GetKeyState('G') & 0x8000)
			offset += 100;

		GetLocalTime(&sysTime);
		sysTimeSec = (sysTime.wHour * 3600 + sysTime.wMinute * 60 + sysTime.wSecond + offset) % 84600;
		updateSkyColor();

		sphericalPosition = YVec3<float>(sphericalPosition.X, (float)sysTimeSec / 84600.0f * TWO_PI + TWO_PI/2, sphericalPosition.Y);
		updateDirection();
	}

	void createVbo()
	{
		vbo = shape.createVbo();
	}

	void updateSkyColor()
	{
		if (sysTimeSec < 18000)
			skyColor = skyColorPalette[0];
		else if (sysTimeSec < 21600)
			skyColor = skyColorPalette[0].interpolate(skyColorPalette[1], (float)(sysTimeSec - 18000) / 3600.0);
		else if (sysTimeSec < 25200)
			skyColor = skyColorPalette[1].interpolate(skyColorPalette[2], (float)(sysTimeSec - 21600) / 3600.0);
		else if (sysTimeSec < 64800)
			skyColor = skyColorPalette[2];
		else if (sysTimeSec < 68400)
			skyColor = skyColorPalette[2].interpolate(skyColorPalette[3], (float)(sysTimeSec - 64800) / 3600.0);
		else if (sysTimeSec < 72000)
			skyColor = skyColorPalette[3].interpolate(skyColorPalette[0], (float)(sysTimeSec - 68400) / 3600.0);
		else
			skyColor = skyColorPalette[0];
	}

	void render()
	{
		YRenderer& renderer = *YEngine::getInstance()->Renderer;
		YVec3<float> sunPos = getPosition();

		glUseProgram(shader);
		glPushMatrix();
		glTranslatef(sunPos.X, sunPos.Y, sunPos.Z);
		glScalef(size.X, size.Y, size.Z);
		renderer.updateMatricesFromOgl();
		renderer.sendMatricesToShader(shader);
		vbo->render();
		glPopMatrix();
	}

	YVec3<float> fromSphericalCoordinates(const float dist, const float theta, const float phi)
	{
		float x = dist * sin(theta) * cos(phi);
		float y = dist * sin(theta) * sin(phi);
		float z = dist * cos(theta);
		return YVec3<float>(x, y, z);
	}

private:
	YVbo* vbo;
	CubeShape shape;

	YColor skyColorPalette[4] = {
		YColor(2.0f / 255.0f,  13.0f / 255.0f,  31.0f / 255.0f, 1.0f),
		YColor(250.0f / 255.0f, 123.0f / 255.0f,  98.0f / 255.0f, 1.0f),
		YColor(174.0f / 255.0f, 232.0f / 255.0f, 245.0f / 255.0f, 1.0f),
		YColor(80.0f / 255.0f,  58.0f / 255.0f,  44.0f / 255.0f, 1.0f),
	};
};