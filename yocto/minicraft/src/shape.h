#pragma once

#include <vector>
#include <array>
#include <memory>
#include <bit>

#include "engine/render/vbo.h"
#include "engine/utils/types_3d.h"

struct GeometryVertex
{
	YVec3<float> position;
	YVec3<float> normal;
	YVec2<float> uv;
	unsigned int type;
};

class CubeShape
{
private:
	const std::array<YVec3<float>, 8> vertices{
		YVec3<float>(-0.5, -0.5, -0.5), //A
		YVec3<float>(0.5, -0.5, -0.5), //B
		YVec3<float>(0.5, -0.5,  0.5), //C
		YVec3<float>(-0.5, -0.5,  0.5), //D
		YVec3<float>(-0.5,  0.5, -0.5), //E
		YVec3<float>(0.5,  0.5, -0.5), //F
		YVec3<float>(0.5,  0.5,  0.5), //G
		YVec3<float>(-0.5,  0.5,  0.5)  //H
	};

	std::vector<std::array<YVec3<float>, 4>> geometry;
	std::vector<YVec3<float>> normals;
public:
	enum Face
	{
		North = 1 << 0,
		East = 1 << 1,
		South = 1 << 2,
		West = 1 << 3,
		Top = 1 << 4,
		Bottom = 1 << 5,
	};

	const static unsigned int ALL_FACES = North | East | South | West | Top | Bottom;
	const static unsigned int NO_FACES = 0;

	CubeShape(unsigned int faces)
	{
		geometry = std::vector<std::array<YVec3<float>, 4>>();
		normals = std::vector<YVec3<float>>();

		geometry.reserve(std::popcount(faces));
		normals.reserve(geometry.capacity());

		for (size_t face = 1; face < (1 << 6); face <<= 1)
		{
			if ((face & faces) == 0)
				continue;

			geometry.push_back(createFace((Face)face));
			normals.push_back(getFaceNormal((Face)face));
		}
	};

	std::vector<GeometryVertex> createVboGeometry()
	{
		std::vector<GeometryVertex> vertices{};
		vertices.reserve(geometry.size() * 6);

#pragma omp parallel
		for (size_t i = 0; i < geometry.size(); i++)
		{
			vertices.push_back({ 
				{geometry[i][0].X, geometry[i][0].Y, geometry[i][0].Z},
				{normals[i].X, normals[i].Y, normals[i].Z}, 
				{0, 0}
				});

			vertices.push_back({
				{geometry[i][1].X, geometry[i][1].Y, geometry[i][1].Z},
				{normals[i].X, normals[i].Y, normals[i].Z},
				{1, 0}
				});

			vertices.push_back({
				{geometry[i][2].X, geometry[i][2].Y, geometry[i][2].Z},
				{normals[i].X, normals[i].Y, normals[i].Z},
				{1, 1}
				});

			vertices.push_back({
				{geometry[i][0].X, geometry[i][0].Y, geometry[i][0].Z},
				{normals[i].X, normals[i].Y, normals[i].Z},
				{0, 0}
				});

			vertices.push_back({
				{geometry[i][2].X, geometry[i][2].Y, geometry[i][2].Z},
				{normals[i].X, normals[i].Y, normals[i].Z},
				{1, 1}
				});

			vertices.push_back({
				{geometry[i][3].X, geometry[i][3].Y, geometry[i][3].Z},
				{normals[i].X, normals[i].Y, normals[i].Z},
				{0, 1}
				});
		}

		return vertices;
	}

	YVbo* createVbo()
	{
		YVbo* vbo = new YVbo(3, geometry.size() * 6, YVbo::PACK_BY_ELEMENT_TYPE);

		vbo->setElementDescription(0, YVbo::Element(3)); // Vertex
		vbo->setElementDescription(1, YVbo::Element(3)); // Normal
		vbo->setElementDescription(2, YVbo::Element(2)); // UV

		vbo->createVboCpu();

		std::vector<GeometryVertex> vboGeometry = createVboGeometry();
		for (size_t i = 0; i < vboGeometry.size(); i++)
		{
			vbo->setElementValue(0, i, vboGeometry[i].position.X, vboGeometry[i].position.Y, vboGeometry[i].position.Z);
			vbo->setElementValue(1, i, vboGeometry[i].normal.X, vboGeometry[i].normal.Y, vboGeometry[i].normal.Z);
			vbo->setElementValue(2, i, vboGeometry[i].uv.X, vboGeometry[i].uv.Y);
		}

		vbo->createVboGpu();

		vbo->deleteVboCpu();

		return vbo;
	}

	constexpr YVec3<float> getFaceNormal(const Face face) const
	{
		switch (face)
		{
		case Face::North:
			return { 0,  1,  0 };
		case Face::East:
			return { 1,  0,  0 };
		case Face::South:
			return { 0, -1,  0 };
		case Face::West:
			return { -1,  0,  0 };
		case Face::Top:
			return { 0,  0,  1 };
		case Face::Bottom:
			return { 0,  0, -1 };
		default:
			return {};
		}
	}

	constexpr std::array<YVec3<float>, 4> createFace(Face face) const
	{
		switch (face)
		{
		case Face::North:
			return { vertices[5], vertices[4], vertices[7], vertices[6] };
		case Face::East:
			return { vertices[1], vertices[5], vertices[6], vertices[2] };
		case Face::South:
			return { vertices[0], vertices[1], vertices[2], vertices[3] };
		case Face::West:
			return { vertices[4], vertices[0], vertices[3], vertices[7] };
		case Face::Top:
			return { vertices[3], vertices[2], vertices[6], vertices[7] };
		case Face::Bottom:
			return { vertices[4], vertices[5], vertices[1], vertices[0] };
		default:
			return {};
		}
	}
};