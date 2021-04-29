#pragma once
#include <unordered_map>
#include "helpers.h"

class VertexBufferHeader;

class VertexBuffer : public File
{
private:
	VertexBufferHeader* header = nullptr;

public:
	VertexBuffer(std::string x, VertexBufferHeader* h) : File(x)
	{
		header = h;
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

	VertexBufferHeader(std::string x, BufferType type) : Header(x)
	{
		getData();
		getHeader(x);
		vertexBuffer = new VertexBuffer(getReferenceFromHash(x), this);
		type = type;
	}

	VertexBufferHeader(std::string x) : Header(x)
	{
	}

};