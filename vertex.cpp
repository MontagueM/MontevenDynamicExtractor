#include "vertex.h"

void VertexBufferHeader::getHeader(std::string x)
{
	memcpy((char*)&stride, data + 4, 2);
}

void VertexBuffer::getVerts(Mesh* mesh)
{
	int fileSize = getData();
	int s = header->stride;
	if (header->type == VertPrimary)
	{
		// Read position data float16
		if (s == 8 || s == 12 || s == 16 || s == 24)
		{
			for (int i = 0; i < fileSize; i += s)
			{
				std::vector<float> vertex;
				for (int j = 0; j < 3; j++)
				{
					int16_t num;
					memcpy((char*)&num, data + i + j*4, 2);
					vertex.push_back(num / 32767);
				}
				mesh->vertPos.push_back(vertex);
			}
		}
	}
	else if (header->type == VertSecondary)
	{
		// UV data
	}
	else
	{
		printf("Implement more vert types dumbo");
	}
}