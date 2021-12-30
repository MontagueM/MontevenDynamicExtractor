#include "vertex.h"

void VertexBufferHeader::getHeader(std::string x)
{
	memcpy((char*)&stride, data + 4, 2);
}

void VertexBuffer::getVerts(DynamicMesh* mesh)
{
	// TODO implement stride 48 float32, normals
	int fileSize = getData();
	int s = stride;
	int16_t num;
	bool bIsWeights = false;
	if (type == VertPrimary)
	{
		// Read position data float16
		if (s == 8 || s == 12 || s == 16 || s == 24 || s == 28 || s == 32)
		{
			if (s == 12)
			{
				// It can be either UVs or weights, need to identify
				int w1 = 0;
				int w2 = 0;
				uint8_t val;

				for (int j = 10; j < 12; j++)
				{
					memcpy((char*)&val, data + j, 1);
					w1 += val;
					memcpy((char*)&val, data + j + 12, 1);
					w2 += val;
				}
				if (w1 == 255 && w2 == 255)
				{
					bIsWeights = true;
				}
			}

			for (int i = 0; i < fileSize; i += s)
			{
				readPosition(mesh, i, data);
				memcpy((char*)&num, data + 6, 2);
				if (num != 32767)
				{
					readWeights1(mesh, i + 6, data);
				}

				if (s == 12)
				{
					if (bIsWeights)
					{
						readWeights2(mesh, i + 0x8, data);
					}
					else
					{
						readUV(mesh, i + 0x8, data);
					}
				}
				else if (s == 16)
				{
					readWeights4(mesh, i + 0x8, data);
				}
				else if (s == 24)
				{
					// No clue, UV
					readUV(mesh, i + 0x14, data);
				}
				else if (s == 28)
				{
					// UV, Normal, Tangent
					readUV(mesh, i + 0x8, data);
					readNormal(mesh, i + 0xC, data);
				}
				else if (s == 32)
				{
					// UV, Normal, Tangent, VC
					readUV(mesh, i + 0x8, data);
					readNormal(mesh, i + 0xC, data);
					readVertexColour(mesh, i + 0x1C, data);
				}
			}
		}

		if (s == 48)
		{
			float fnum;
			for (int i = 0; i < fileSize; i += s)
			{
				std::vector<float> vertex;
				vertex.reserve(3);
				for (int j = 0; j < 3; j++)
				{
					memcpy((char*)&fnum, data + i + j * 4, 4);
					vertex.push_back(fnum);
				}
				mesh->vertPos.push_back(vertex);

				std::vector<float> norm;
				norm.reserve(3);
				for (int j = 0; j < 3; j++)
				{
					memcpy((char*)&fnum, data + i + 0x10 + j * 4, 4);
					norm.push_back(fnum);
				}
				mesh->vertNorm.push_back(norm);
			}
		}

		if (mesh->vertPos.size() == 0)
		{
			exit(77);
		}
	}
	else if (type == VertSecondary)
	{
		bool bUVExists = mesh->vertUV.size() != 0;
		// UV data
		if (s == 4 || s == 8 || s == 12 || s == 16 || s == 20 || s == 24 || s == 28)
		{
			for (int i = 0; i < fileSize; i += s)
			{
				if (s == 16)
				{
					readUV(mesh, i + 0x8, data);
				}
				else if (s == 8)
				{
					readNormal(mesh, i, data);
				}
				else if (s == 4)
				{
					readUV(mesh, i, data);
				}


				if (s == 12 || s == 20)
				{
					// If s == 20 the UV can be in here unless its already been defined
					if (s == 20)
					{
						if (!bUVExists)
						{
							readUV(mesh, i, data);
							readNormal(mesh, i + 0x4, data);
						}
						else
						{
							readNormal(mesh, i, data);
							readVertexColour(mesh, i + 0x10, data);
						}
					}
					else
					{
						readUV(mesh, i, data);
						readNormal(mesh, i + 0x4, data);
					}
					// 20 is just tangent as well
				}
				else if (s == 24)
				{
					// UV, normal, tangent, vertex colour
					readUV(mesh, i, data);
					readNormal(mesh, i + 0x4, data);
					readVertexColour(mesh, i + 0x14, data);
				}
				else if (s == 28)
				{
					// UV, normal, tangent, unk
					readUV(mesh, i, data);
					readNormal(mesh, i + 0x4, data);
				}
			}
		}
		if (mesh->vertUV.size() == 0)
		{
			exit(78);
		}
	}
	else if (type == OldWeights)
	{
		// Weights for cloth
		if (s == 8)
		{
			for (int i = 0; i < fileSize; i += s)
			{
				readWeights4(mesh, i, data);
			}
		}
		if (mesh->weights.size() == 0)
		{
			exit(79);
		}
	}
	else
	{
		printf("Implement more vert types dumbo");
		exit(14);
	}
}


void VertexBuffer::readPosition(DynamicMesh* mesh, int i, unsigned char* data)
{
	std::vector<float> vertex;
	vertex.reserve(3);
	int16_t num;
	for (int j = 0; j < 3; j++)
	{
		memcpy((char*)&num, data + i + j * 2, 2);
		vertex.push_back((float)num / 32767);
	}
	mesh->vertPos.push_back(vertex);
}

void VertexBuffer::readUV(DynamicMesh* mesh, int i, unsigned char* data)
{
	std::vector<float> vertex;
	vertex.reserve(2);
	int16_t num;
	for (int j = 0; j < 2; j++)
	{
		memcpy((char*)&num, data + i + j * 2, 2);
		vertex.push_back((float)num / 32767);
	}
	mesh->vertUV.push_back(vertex);
}

void VertexBuffer::readNormal(DynamicMesh* mesh, int i, unsigned char* data)
{
	std::vector<float> norm;
	norm.reserve(3);
	int16_t num;
	for (int j = 0; j < 3; j++)
	{
		memcpy((char*)&num, data + i + j * 2, 2);
		norm.push_back((float)num / 32767);
	}
	mesh->vertNorm.push_back(norm);
}

void VertexBuffer::readWeights1(DynamicMesh* mesh, int i, unsigned char* data)
{
	uint8_t val;
	// Weight indices
	std::vector<uint8_t> wi;
	memcpy((char*)&val, data + i, 1);
	wi.push_back(val);

	// Weights
	std::vector<float> w;
	w.push_back(1.f);

	mesh->weightIndices.push_back(wi);
	mesh->weights.push_back(w);
}

void VertexBuffer::readWeights2(DynamicMesh* mesh, int i, unsigned char* data)
{
	uint8_t val;
	// Weight indices
	std::vector<uint8_t> wi;
	for (int j = i; j < i + 2; j++)
	{
		memcpy((char*)&val, data + j, 1);
		if (val != 255) wi.push_back(val);
	}

	// Weights
	std::vector<float> w;
	for (int j = i + 2; j < i + 2 + wi.size(); j++)
	{
		memcpy((char*)&val, data + j, 1);
		w.push_back((float)val / 255);
	}
	mesh->weightIndices.push_back(wi);
	mesh->weights.push_back(w);
}

void VertexBuffer::readWeights4(DynamicMesh* mesh, int i, unsigned char* data)
{
	uint8_t val;
	// Weight indices
	std::vector<uint8_t> wi;
	for (int j = i + 4; j < i + 8; j++)
	{
		memcpy((char*)&val, data + j, 1);
		if (val != 255) wi.push_back(val);
	}

	// Weights
	std::vector<float> w;
	for (int j = i; j < i + wi.size(); j++)
	{
		memcpy((char*)&val, data + j, 1);
		w.push_back((float)val / 255);
	}
	mesh->weightIndices.push_back(wi);
	mesh->weights.push_back(w);
}

void VertexBuffer::readVertexColour(DynamicMesh* mesh, int i, unsigned char* data)
{
	uint8_t val;
	std::vector<float> vc;
	for (int j = i; j < i + 4; j++)
	{
		memcpy((char*)&val, data + j, 1);
		vc.push_back((float)val / 255);
	}
	mesh->vertCol.push_back(vc);
}
