#pragma once
#include <unordered_map>
#include "helpers.h"

class VertexBufferHeader : public Header
{
private:
	void getHeader();

public:
	uint16_t stride;
	BufferType type;

	VertexBufferHeader(std::string x, BufferType type) : Header(x)
	{
		getData();
		getHeader();
		VertexBuffer vertexBuffer = VertexBuffer(getReferenceFromHash(x), this);
		type = type;
	}

};

class VertexBuffer : public File
{
private:
	VertexBufferHeader* header;

public:
	VertexBuffer(std::string x, VertexBufferHeader* h) : File(x)
	{
		header = h;
	}

	void getVerts(Mesh mesh);
};