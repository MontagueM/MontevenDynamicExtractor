#pragma once
#include <unordered_map>
#include "helpers.h"

class IndexBufferHeader : public Header
{
private:
	void getHeader();

public:
	int stride = 2;

	IndexBufferHeader(std::string x) : Header(x)
	{
		getData();
		getHeader();
		IndexBuffer indexBuffer = IndexBuffer(getReferenceFromHash(x), this);
	}

};

class IndexBuffer : public File
{
private:
	IndexBufferHeader* header;

public:
	IndexBuffer(std::string x, IndexBufferHeader* h) : File(x)
	{
		header = h;
	}

	void getFaces(Mesh mesh, PrimitiveType primType);
};