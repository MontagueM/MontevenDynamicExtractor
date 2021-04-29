#include "index.h"


void IndexBufferHeader::getHeader(std::string x)
{
	bool bIsStride4;
	memcpy((char*)&bIsStride4, data + 0x1, 1);
	if (bIsStride4) stride = 4;

	
}

void IndexBuffer::getFaces(Mesh* mesh, PrimitiveType primType)
{
	int fileSize = getData();
	int increment = 3;
	std::vector<uint32_t> intFacesData;
	intFacesData.reserve((floor(fileSize / stride)));

	for (int i = 0; i < fileSize; i += stride)
	{
		uint32_t face = 0;
		memcpy((char*)&face, data + i, stride);
		intFacesData.push_back(face);
	}
	if (primType == TriangleStrip) increment = 1;

	int j = 0;
	int faceIndex = 0;
	while (true)
	{
		if (faceIndex >= intFacesData.size() - 2 && primType == TriangleStrip)
		{
			mesh->faceMap[faceIndex] = mesh->faces.size() - 1;
			if (faceIndex == intFacesData.size())
			{
				mesh->faceMap[faceIndex + 1] = mesh->faces.size() - 1;
				break;
			}
			faceIndex++;
			continue;
		}
		else if (faceIndex == intFacesData.size() && primType == Triangles)
		{
			mesh->faceMap[faceIndex] = mesh->faces.size() - 1;
			break;
		}
		// Check vector break
		bool bEnd = false;
		for (int i = 0; i < 3; i++)
		{
			if (intFacesData[faceIndex + i] == 65535 or intFacesData[faceIndex + i] == 4294967295)
			{
				bEnd = true;
				break;
			}
		}
		if (bEnd)
		{
			j = 0;
			mesh->faceMap[faceIndex] = mesh->faces.size() - 1;
			faceIndex += increment;
			continue;
		}

		std::vector<uint32_t> face;
		if (primType == Triangles || j % 2 == 0)
			face = std::vector<uint32_t>(intFacesData.begin() + faceIndex, intFacesData.begin() + faceIndex + 3);
		else
			face = {intFacesData[faceIndex + 1], intFacesData[faceIndex], intFacesData[faceIndex + 2] };
		mesh->faces.push_back(face);
		mesh->faceMap[faceIndex] = mesh->faces.size() - 1;
		faceIndex += increment;
		j++;
	}
}