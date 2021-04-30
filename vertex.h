#pragma once
#include <unordered_map>
#include "helpers.h"

class VertexBuffer : public File
{
private:

public:
	uint16_t stride;
	BufferType type;

	VertexBuffer(std::string x, std::string pkgsPath, uint16_t s, BufferType type) : File(x, pkgsPath)
	{
		stride = s;
		type = type;
	}

	void getVerts(Mesh* mesh);
};

class VertexBufferHeader : public Header
{
private:
	void getHeader(std::string x);

public:
	uint16_t stride;
	BufferType type;
	VertexBuffer* vertexBuffer = nullptr;

	VertexBufferHeader(std::string x, std::string pkgsPath, BufferType type) : Header(x, pkgsPath)
	{
		getData();
		getHeader(x);
		vertexBuffer = new VertexBuffer(getReferenceFromHash(x, packagesPath), packagesPath, stride, type);
		type = type;
	}

	VertexBufferHeader(std::string x, std::string pkgsPath) : Header(x, pkgsPath)
	{
	}

};