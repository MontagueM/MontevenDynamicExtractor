#pragma once
#include <unordered_map>
#include "helpers.h"


class IndexBuffer : public File
{

public:
	int stride = -1;

	IndexBuffer(std::string x, std::string pkgsPath, int s) : File(x, pkgsPath)
	{
		stride = s;
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

	IndexBufferHeader(std::string x, std::string pkgsPath) : Header(x, pkgsPath)
	{
		if (x != "")
		{
			getData();
			getHeader(x);
			indexBuffer = new IndexBuffer(getReferenceFromHash(x, pkgsPath), pkgsPath, stride);
		}
		auto a = 0;
	}

};