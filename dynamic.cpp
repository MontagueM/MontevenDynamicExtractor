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
}

void Dynamic::getSubmeshes()
{

}