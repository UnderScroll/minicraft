#pragma once

#include <array>
#include <memory>

#include "engine/render/vbo.h"
#include "engine/utils/types_3d.h"

enum Face;

class CubeShape
{
private:
	std::array<int, 36> gemoetry;
	Face faces;
public:
	CubeShape(Face faces) {

	};

	void getFaceGeometry(Face face)
	{

	}

	std::unique_ptr<YVbo> createVbo()
	{
		std::unique_ptr<YVbo> vbo = std::make_unique<YVbo>(YVbo(3, 36, YVbo::PACK_BY_ELEMENT_TYPE));
		YVbo& r_vbo = *vbo;

		vbo->setElementDescription(0, YVbo::Element(3)); // Vertex
		vbo->setElementDescription(1, YVbo::Element(3)); // Normal
		vbo->setElementDescription(2, YVbo::Element(2)); // UV

		return vbo;
	}

	enum Face
	{
		North = 1 << 0,
		East = 1 << 1,
		South = 1 << 2,
		West = 1 << 3,
		Top = 1 << 4,
		Bottom = 1 << 5,
	};
};