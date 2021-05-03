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
	if (type == VertPrimary)
	{
		// Read position data float16
		if (s == 8 || s == 12 || s == 16 || s == 24)
		{
			for (int i = 0; i < fileSize; i += s)
			{
				std::vector<float> vertex;
				vertex.reserve(3);
				for (int j = 0; j < 3; j++)
				{
					memcpy((char*)&num, data + i + j*2, 2);
					vertex.push_back((float)num / 32767);
				}
				mesh->vertPos.push_back(vertex);
			}
		}
		// float16 vert W
		if (s == 24)
		{
			for (int i = 0; i < fileSize; i += s)
			{
				std::vector<float> norm;
				norm.reserve(3);
				for (int j = 0; j < 3; j++)
				{
					memcpy((char*)&num, data + i + 8 + j * 2, 2);
					norm.push_back((float)num / 32767);
				}
				mesh->vertNorm.push_back(norm);

				memcpy((char*)&num, data + i + 6, 2);
				mesh->vertPosW.push_back(num);
				memcpy((char*)&num, data + i + 0xE, 2);
				mesh->vertNormW.push_back(num);
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

				memcpy((char*)&fnum, data + i + 0x1C, 4);
				mesh->vertNormW.push_back(fnum);
			}
		}
	}
	else if (type == VertSecondary)
	{
		// UV data
		if (s == 4 || s == 8 || s == 12 || s == 16 || s == 20 || s == 24)
		{
			for (int i = 0; i < fileSize; i += s)
			{
				std::vector<float> vertex;
				vertex.reserve(2);
				for (int j = 0; j < 2; j++)
				{
					memcpy((char*)&num, data + i + j * 2, 2);
					vertex.push_back((float)num / 32767);
				}
				mesh->vertUV.push_back(vertex);
			}
		}
	}
	else if (type == OldWeights)
	{
		getOldWeights(mesh, fileSize);
	}
	else if (type == VertColour)
	{
		getVertexColour(mesh, fileSize);
	}
	else if (type == SPSBWeights)
	{
		getSPSBWeights(mesh, fileSize);
	}
	else
	{
		printf("Implement more vert types dumbo");
	}
}

void VertexBuffer::getOldWeights(DynamicMesh* mesh, int fileSize)
{
	for (int i = 0; i < fileSize; i += 8)
	{
		uint8_t val;

		// Weight indices
		std::vector<uint8_t> wi;
		for (int j = i + 4; j < i + 8; j++)
		{
			memcpy((char*)&val, data + j, 1);
			if (val != 254) wi.push_back(val);
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
}

void VertexBuffer::getVertexColour(DynamicMesh* mesh, int fileSize)
{
	bool bAnyValidVC = false;
	for (int i = 0; i < fileSize; i += 4)
	{
		uint8_t val;
		std::vector<float> vc;
		for (int j = i; j < i + 4; j++)
		{
			memcpy((char*)&val, data + j, 1);
			if (!bAnyValidVC)
			{
				if (val != 0) bAnyValidVC = true;
			}
			vc.push_back((float)val / 255);
		}
		//if (vc[3] != 1.0) vc = { 0, 0, 0, 0 };
		mesh->vertCol.push_back(vc);
	}
	if (!bAnyValidVC) mesh->vertCol.clear();
}

void VertexBuffer::getSPSBWeights(DynamicMesh* mesh, int fileSize)
{
	bool bInHeader = false;
	for (auto& w : mesh->vertPosW)
	{
		if (w & 0xf800)
		{
			bInHeader = true;
			break;
		}
	}
	int headerOffset = 0;
	if (bInHeader)
	{
		if (fileSize < 32) return;

		// Finding header end
		int i = 0;
		while (true)
		{
			int32_t comp1, comp2, comp3;
			memcpy((char*)&comp1, data + i, 2);
			memcpy((char*)&comp2, data + i + 2, 2);
			memcpy((char*)&comp3, data + i, 4);
			// Check if we're in header, there are often zero values or same value for both
			if (comp1 == comp2 || comp3 == 0)
			{
				bInHeader = true;
				i = 32 * i / 32 + 32;
			}
			else
			{
				bInHeader = false;
				i += 4;
			}
			if (i % 32 == 0 && i != 0 && !bInHeader)
			{
				headerOffset = i - 64;
				break;
			}
			if (i >= fileSize) return;
		}
	}

	int blendIndex;
	int blendFlags;
	int bufferSize;
	int lastBlendValue = 0;
	int lastBlendCount = 0;

	for (auto& w : mesh->vertPosW)
	{
		std::vector<uint8_t> indices = { 0, 0, 0, 0 };
		std::vector<float> weights = { 1., 0., 0., 0. };

		blendIndex = w & 0x7ff;
		blendFlags = w & 0xf800;
		bufferSize = 0;

		if (blendFlags & 0x8000)
		{
			blendIndex = abs(w) - 2048;
			bufferSize = 4;
		}
		else if (blendFlags == 0x800)
		{
			bufferSize = 2;
		}
		else if (blendFlags & 0x1000 || blendFlags & 0x4000)
		{
			bufferSize = 2;
			blendIndex = abs(w) - 2048;
		}
		else if (blendFlags == 0)
		{
			indices[0] = blendIndex;
		}
		else
		{
			printf("Unk flag used in skin buffer.");
			return;
		}

		if (bufferSize > 0)
		{
			if (lastBlendValue != blendIndex) lastBlendCount = 0;

			lastBlendValue = blendIndex;

			for (int i = 0; i < bufferSize; i += 2)
			{
				while (blendIndex * 32 + lastBlendCount + i * 2 <= headerOffset)
				{
					lastBlendCount += 32;
				}
				uint8_t w0;
				uint8_t w1;
				memcpy((char*)&indices[i], data + blendIndex * 32 + lastBlendCount + i * 2, 1);
				memcpy((char*)&indices[i + 1], data + blendIndex * 32 + 1 + lastBlendCount + i * 2, 1);
				memcpy((char*)&w0, data + blendIndex * 32 + 2 + lastBlendCount + i * 2, 1);
				memcpy((char*)&w1, data + blendIndex * 32 + 3 + lastBlendCount + i * 2, 1);
				weights[i]  = (float)w0 / 255;
				weights[i+1] = (float)w1 / 255;

			}
			lastBlendCount += bufferSize * 2;
		}
		// Slow
		std::vector<float> newWeights;
		for (auto& w : weights)
		{
			if (w != 0) newWeights.push_back(w);
		}
		// Might not work?
		indices.resize(newWeights.size());
		mesh->weightIndices.push_back(indices);
		mesh->weights.push_back(weights);
	}
}