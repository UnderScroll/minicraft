#pragma once

#include <map>

#include "engine/render/vbo.h"
#include "shape.h"
#include <engine/noise/perlin.h>

static const unsigned int CHUNK_SIZE = 64;
static const unsigned int TERRAIN_HEIGHT = 10;
static const unsigned int WATER_LEVEL = 16;

struct Block
{
public:
	enum Type
	{
		Air = 0,
		Grass,
		Dirt,
		Stone,
		Water
	};

	const Type type;
	const bool transparent;

	static const Block* get(const Type type)
	{
		auto it = blockMap.find(type);
		if (it == blockMap.end())
			return nullptr;

		return &it->second;
	};
private:
	Block(Type type, bool trans)
		:type(type), transparent(trans)
	{
	}

	static const std::map<Type, Block> blockMap;
};

const std::map<Block::Type, Block> Block::blockMap = {
	{ Type::Air, {Type::Air, true }},
	{ Type::Grass, {Type::Grass, false} },
	{ Type::Dirt, {Type::Dirt, false} },
	{ Type::Stone, {Type::Stone, false} },
	{ Type::Water, {Type::Water, true} },
};


class Terrain
{
public:
	class Chunk
	{
	public:
		YVec3<int> position;

		Chunk()
			:Chunk(YVec3<int>(0, 0, 0))
		{
		}

		Chunk(YVec3<int> position)
			:position(position)
		{
			blocks = std::vector<const Block*>();
			blocks.reserve(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
			for (size_t x = 0; x < CHUNK_SIZE; x++)
				for (size_t y = 0; y < CHUNK_SIZE; y++)
					for (size_t z = 0; z < CHUNK_SIZE; z++)
					{
						blocks.push_back(Block::get(Block::Air));
					}

			cubes = std::vector<CubeShape>();
			blocks.reserve(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
		}

		void setBlock(const Block* block, size_t x, size_t y, size_t z)
		{
			if (isInBounds(x, y, z))
				blocks[x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + z] = block;
		}

		const Block* getBlock(size_t x, size_t y, size_t z)
		{
			if (isInBounds(x, y, z))
				return blocks[x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + z];
			return Block::get(Block::Air);
		}

		void generate(unsigned int seed)
		{
			srand(seed);

			//Height
			YPerlin perlin{};
#pragma omp parallel
			{
#pragma omp for
				for (size_t x = 0; x < CHUNK_SIZE; x++)
				{
					int world_x = x + position.X * CHUNK_SIZE;
#pragma omp for
					for (size_t y = 0; y < CHUNK_SIZE; y++)
					{
						int world_y = y + position.Y * CHUNK_SIZE;
						float height = perlin.sample((float)world_x / 15.0, (float)world_y / 15.0, 0);
						float mound = perlin.sample((float)world_x / 40.0, (float)world_y / 40.0, 0) * 3 - 0.5;
						float mound_jagged = (perlin.sample((float)world_x / 10.0, (float)world_y / 10.0, 0) - 0.7) * 4 + 0.7;
						mound_jagged = mound_jagged > 0 ? mound_jagged : 0;
						mound = mound * mound * mound;
						mound = mound > 0.9 ? mound + mound_jagged : mound;
						height += mound;
						for (size_t z = 0; z < CHUNK_SIZE; z++)
						{
							int world_z = z + position.Z * CHUNK_SIZE;
							if (height > (float)world_z / TERRAIN_HEIGHT)
								setBlock(Block::get(Block::Stone), x, y, z);
							else if (height > (float)(world_z - 3) / TERRAIN_HEIGHT)
								setBlock(Block::get(Block::Dirt), x, y, z);
							else if (height > (float)(world_z - 4) / TERRAIN_HEIGHT)
								setBlock(Block::get(Block::Grass), x, y, z);
							else
								break;
						}
					}
				}
			}


			//Water
#pragma omp parallel
			{
#pragma omp for
				for (size_t x = 0; x < CHUNK_SIZE; x++)
#pragma omp for
					for (size_t y = 0; y < CHUNK_SIZE; y++)
#pragma omp for
						for (size_t z = 0; z < CHUNK_SIZE; z++)
						{
							int world_z = z + position.Z * CHUNK_SIZE;
							if (world_z < WATER_LEVEL)
								if (getBlock(x, y, z)->type == Block::Type::Air)
									setBlock(Block::get(Block::Water), x, y, z);
								else if (world_z + 1!= WATER_LEVEL && getBlock(x, y, z)->type == Block::Type::Grass)
									setBlock(Block::get(Block::Dirt), x, y, z);
						}
			}

			//Caves
#pragma omp parallel
			{
#pragma omp for
				for (size_t x = 0; x < CHUNK_SIZE; x++)
				{
					int world_x = x + position.X * CHUNK_SIZE;
#pragma omp for
					for (size_t y = 0; y < CHUNK_SIZE; y++)
					{
						int world_y = y + position.Y * CHUNK_SIZE;
#pragma omp for
						for (size_t z = 0; z < CHUNK_SIZE; z++)
						{
							int world_z = z + position.Z * CHUNK_SIZE;
							float inCave = perlin.sample((float)world_x / 15.0, (float)world_y / 15.0, (float)world_z / 15.0);
							if (world_z <= (float)TERRAIN_HEIGHT / 2.0)
								inCave *= (double)world_z / ((double)world_z + (double)TERRAIN_HEIGHT * 0.02) - 0.01;
							else
								inCave *= (float)TERRAIN_HEIGHT * 10 / (float)(world_z + TERRAIN_HEIGHT * 10);
							if (inCave > 0.5 && getBlock(x, y, z)->type != Block::Water)
								setBlock(Block::get(Block::Air), x, y, z);
						}
					}
				}
			}

		}

		YVbo* buildVboTransparent()
		{
			std::vector<GeometryVertex> vertices;

			//Create geometry
#pragma omp parallel
			{
#pragma omp for
				for (size_t x = 0; x < CHUNK_SIZE; x++)
#pragma omp for
					for (size_t y = 0; y < CHUNK_SIZE; y++)
#pragma omp for
						for (size_t z = 0; z < CHUNK_SIZE; z++)
						{
							const Block* block = getBlock(x, y, z);

							if (block->type == Block::Air || !block->transparent)
								continue;

							unsigned int faces = CubeShape::NO_FACES;

							const Block* northtblock = getBlock(x, y + 1, z);
							const Block* eastblock = getBlock(x + 1, y, z);
							const Block* southblock = getBlock(x, y - 1, z);
							const Block* westblock = getBlock(x - 1, y, z);
							const Block* topblock = getBlock(x, y, z + 1);
							const Block* bottomblock = getBlock(x, y, z - 1);

							faces |= (!(bool)northtblock->type | !northtblock->transparent) << 0;//North
							faces |= (!(bool)eastblock->type | !eastblock->transparent) << 1;	//East
							faces |= (!(bool)southblock->type | !southblock->transparent) << 2;	//South
							faces |= (!(bool)westblock->type | !westblock->transparent) << 3;	//West
							faces |= (!(bool)topblock->type | !topblock->transparent) << 4;		//Top
							faces |= (!(bool)bottomblock->type | !bottomblock->transparent) << 5;//Bottom

							CubeShape shape = CubeShape(faces);
							std::vector<GeometryVertex> shapeVertices = shape.createVboGeometry();

							for (GeometryVertex& v : shapeVertices)
							{
								v.position += YVec3<float>(position.X, position.Y, position.Z) * CHUNK_SIZE + YVec3<float>(x, y, z);
								v.type = (unsigned int)block->type;
							}

							vertices.insert(vertices.end(), shapeVertices.begin(), shapeVertices.end());
						}
			}


			//Init Vbo
			YVbo* vbo = new YVbo(4, vertices.size(), YVbo::PACK_BY_ELEMENT_TYPE);

			vbo->setElementDescription(0, YVbo::Element(3)); // Vertex
			vbo->setElementDescription(1, YVbo::Element(3)); // Normal
			vbo->setElementDescription(2, YVbo::Element(2)); // UV
			vbo->setElementDescription(3, YVbo::Element(1)); // Type

			vbo->createVboCpu();

			//Pass geometry to Vbo
#pragma parallel
			for (size_t i = 0; i < vertices.size(); i++)
			{
				vbo->setElementValue(0, i, vertices[i].position.X, vertices[i].position.Y, vertices[i].position.Z);
				vbo->setElementValue(1, i, vertices[i].normal.X, vertices[i].normal.Y, vertices[i].normal.Z);
				vbo->setElementValue(2, i, vertices[i].uv.X, vertices[i].uv.Y);
				vbo->setElementValue(3, i, (unsigned int)vertices[i].type);
			}

			vbo->createVboGpu();

			vbo->deleteVboCpu();

			return vbo;
		}

		YVbo* buildVboOpaque()
		{
			std::vector<GeometryVertex> vertices;

			//Create geometry
#pragma omp parallel
			{
#pragma omp for
				for (size_t x = 0; x < CHUNK_SIZE; x++)
#pragma omp for
					for (size_t y = 0; y < CHUNK_SIZE; y++)
						for (size_t z = 0; z < CHUNK_SIZE; z++)
						{
							const Block* block = getBlock(x, y, z);

							if (block->type == Block::Air || block->transparent)
								continue;

							unsigned int faces = CubeShape::NO_FACES;

							const Block* northtblock = getBlock(x, y + 1, z);
							const Block* eastblock = getBlock(x + 1, y, z);
							const Block* southblock = getBlock(x, y - 1, z);
							const Block* westblock = getBlock(x - 1, y, z);
							const Block* topblock = getBlock(x, y, z + 1);
							const Block* bottomblock = getBlock(x, y, z - 1);

							faces |= (!(bool)northtblock->type| northtblock->transparent) << 0;//North
							faces |= (!(bool)eastblock->type | eastblock->transparent) << 1;	//East
							faces |= (!(bool)southblock->type | southblock->transparent) << 2;	//South
							faces |= (!(bool)westblock->type | westblock->transparent) << 3;	//West
							faces |= (!(bool)topblock->type | topblock->transparent) << 4;		//Top
							faces |= (!(bool)bottomblock->type | bottomblock->transparent) << 5;//Bottom

							CubeShape shape = CubeShape(faces);
							std::vector<GeometryVertex> shapeVertices = shape.createVboGeometry();

							for (GeometryVertex& v : shapeVertices)
							{
								v.position += YVec3<float>(position.X, position.Y, position.Z) * CHUNK_SIZE + YVec3<float>(x, y, z);
								v.type = (unsigned int)block->type;
							}

							vertices.insert(vertices.end(), shapeVertices.begin(), shapeVertices.end());
						}
			}


			//Init Vbo
			YVbo* vbo = new YVbo(4, vertices.size(), YVbo::PACK_BY_ELEMENT_TYPE);

			vbo->setElementDescription(0, YVbo::Element(3)); // Vertex
			vbo->setElementDescription(1, YVbo::Element(3)); // Normal
			vbo->setElementDescription(2, YVbo::Element(2)); // UV
			vbo->setElementDescription(3, YVbo::Element(1)); // Type

			vbo->createVboCpu();

			//Pass geometry to Vbo
#pragma parallel
			for (size_t i = 0; i < vertices.size(); i++)
			{
				vbo->setElementValue(0, i, vertices[i].position.X, vertices[i].position.Y, vertices[i].position.Z);
				vbo->setElementValue(1, i, vertices[i].normal.X, vertices[i].normal.Y, vertices[i].normal.Z);
				vbo->setElementValue(2, i, vertices[i].uv.X, vertices[i].uv.Y);
				vbo->setElementValue(3, i, (unsigned int)vertices[i].type);
			}

			vbo->createVboGpu();

			vbo->deleteVboCpu();

			return vbo;
		}

	private:
		bool isInBounds(size_t x, size_t y, size_t z)
		{
			return x >= 0 && y >= 0 && z >= 0
				&& x < CHUNK_SIZE && y < CHUNK_SIZE && z < CHUNK_SIZE;
		}

		std::vector<const Block*> blocks;
		std::vector<CubeShape> cubes;
	};
};