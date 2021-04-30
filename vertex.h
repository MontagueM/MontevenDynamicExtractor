#pragma once
#include <unordered_map>
#include "helpers.h"

class VertexBuffer : public File
{
private:
	void getOldWeights(DynamicMesh* mesh, int fileSize);
	void getVertexColour(DynamicMesh* mesh, int fileSize);
	void getSPSBWeights(DynamicMesh* mesh, int fileSize);

public:
	uint16_t stride;
	BufferType type;

	VertexBuffer(std::string x, std::string pkgsPath, uint16_t s, BufferType t) : File(x, pkgsPath)
	{
		stride = s;
		type = t;
	}

	void getVerts(DynamicMesh* mesh);
};

class VertexBufferHeader : public Header
{
private:
	void getHeader(std::string x);

public:
	uint16_t stride;
	BufferType type;
	VertexBuffer* vertexBuffer = nullptr;

	VertexBufferHeader(std::string x, std::string pkgsPath, BufferType t) : Header(x, pkgsPath)
	{
		getData();
		getHeader(x);
		type = t;
		vertexBuffer = new VertexBuffer(getReferenceFromHash(x, packagesPath), packagesPath, stride, type);
	}

	VertexBufferHeader(std::string x, std::string pkgsPath) : Header(x, pkgsPath)
	{
	}

};