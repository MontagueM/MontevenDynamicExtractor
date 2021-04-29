#include "dynamic.h"

void Dynamic::get()
{
	getData();
	getDyn3Files();
	parseDyn3s();
	getSubmeshes();
}

void Dynamic::getDyn3Files()
{
	uint32_t off;
	int fileSize;

	uint32_t primFileID;
	memcpy((char*)&primFileID, data + 0xB0, 4);
	File primFile = File(uint32ToHexStr(primFileID));
	fileSize = primFile.getData();

	// Finding 42868080
	bool bSkeleton = false;
	memcpy((char*)&off, primFile.data + 0x18, 4);
	off += 0x18;
	memcpy((char*)&off, primFile.data + off + 4, 4);
	if (off == 2155905501) bSkeleton = true;

	if (bSkeleton)
	{
		memcpy((char*)&off, data + 0xBC, 4);
		dyn2s.push_back(File(uint32ToHexStr(off)));
		memcpy((char*)&off, data + 0xC8, 4);
		dyn2s.push_back(File(uint32ToHexStr(off)));
	}
	else
	{
		memcpy((char*)&off, data + 0xB0, 4);
		dyn2s.push_back(File(uint32ToHexStr(off)));
		memcpy((char*)&off, data + 0xBC, 4);
		dyn2s.push_back(File(uint32ToHexStr(off)));
	}
	memcpy((char*)&off, data + 0xA0, 4);
	if (off == 1) dyn2s.pop_back(); // If the array size is 1 just delete the second dyn2

	std::vector<std::string> existingDyn3s = {};
	for (int i = 0; i < dyn2s.size(); i++)
	{
		File dyn2 = primFile;
		// We don't need to reopen the primFile if there isnt a skeleton
		if (bSkeleton)
		{
			dyn2 = dyn2s[i];
			fileSize = dyn2.getData();
			if (fileSize == 0)
			{
				printf("Model file empty, skipping...");
				return;
			}
		}
		else
		{
			if (i == 1)
			{
				dyn2 = dyn2s[i];
				dyn2.getData();
			}
		}

		memcpy((char*)&off, dyn2.data + 0x18, 4);
		if (off + 572 - 4 >= fileSize)
		{
			printf("Not a valid model file, skipping...");
			return;
		}
		memcpy((char*)&off, dyn2.data + off + 572, 4);
		File dyn3 = File(uint32ToHexStr(off));
		if (std::find(existingDyn3s.begin(), existingDyn3s.end(), dyn3.hash) != existingDyn3s.end()
			|| dyn3.getData() == 0)
			dyn2s.pop_back();
		else
		{
			existingDyn3s.push_back(dyn3.hash);
			dyn3s.push_back(dyn3);
		}
	}
}

void Dynamic::parseDyn3s()
{
	uint32_t offset;
	uint32_t count;
	int fileSize;

	for (int i = 0; i < dyn3s.size(); i++)
	{
		File dyn3 = dyn3s[i];
		// We've already gotten data for them
		memcpy((char*)&offset, dyn3.data + 0x18, 4);
		offset += 0x18 + 0x10;
		memcpy((char*)&count, dyn3.data + 0x10, 4);
		for (int j = offset; j < offset + count * 0x80; i += 0x80)
		{
			DynamicMesh* mesh = new DynamicMesh();
			meshes.push_back(mesh);
			memcpy((char*)&offset, dyn3.data + j+0x10, 4);
			mesh->facesFile = new IndexBufferHeader(uint32ToHexStr(offset));
			mesh->vertPosFile = new VertexBufferHeader(uint32ToHexStr(offset), VertPrimary);
			// rest here

			uint32_t submeshTableCount;
			memcpy((char*)&submeshTableCount, dyn3.data + j + 0x20, 4);
			uint32_t submeshTableOffset;
			memcpy((char*)&submeshTableOffset, dyn3.data + j + 0x28, 4);
			submeshTableOffset += j + 0x28 + 0x10;

			int currentLOD = 999;
			int lodGroup = 0;
			for (int k = submeshTableOffset; k < submeshTableOffset + submeshTableCount * 0x24; k += 0x24)
			{
				std::unique_ptr<DynamicSubmesh> submesh = nullptr;
				memcpy((char*)&submesh->primType, dyn3.data + k + 6, 2);
				memcpy((char*)&submesh->indexOffset, dyn3.data + k + 0x8, 4);
				memcpy((char*)&submesh->indexCount, dyn3.data + k + 0xC, 4);
				memcpy((char*)&submesh->lodLevel, dyn3.data + k + 0x1B, 1);
				if (submesh->lodLevel < currentLOD) lodGroup++;
				currentLOD = submesh->lodLevel;
				submesh->lodGroup = lodGroup;
				mesh->submeshes.push_back(std::move(submesh));
			}

			PrimitiveType primType = mesh->submeshes[0]->primType;

			mesh->vertPosFile->vertexBuffer->getVerts(mesh);
			//transformPos(mesh, dyn3.data);

			mesh->facesFile->indexBuffer->getFaces(mesh, primType);
		}
	}
}

void Dynamic::getSubmeshes()
{

}