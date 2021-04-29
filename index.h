#pragma once
#include <unordered_map>
#include "helpers.h"

class IndexBufferHeader;

class IndexBuffer : public File
{
private:
	IndexBufferHeader* header;

public:
	IndexBuffer(std::string x, IndexBufferHeader* h) : File(x)
	{
		header = h;
	}

	void getFaces(Mesh* mesh, PrimitiveType primType);
};

class IndexBufferHeader : public Header
{
private:
	void getHeader(std::string x);

public:
	int stride = 2;
	IndexBuffer* indexBuffer = nullptr;

	IndexBufferHeader(std::string x) : Header(x)
	{
		if (x != "")
		{
			getData();
			getHeader(x);

		}
		IndexBuffer* indexBuffer = new IndexBuffer(getReferenceFromHash(x), this);
	}

};