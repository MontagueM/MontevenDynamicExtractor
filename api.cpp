#include "api.h"

std::vector<std::string> getAPIModelHashes(uint32_t apiHash, std::string packagesPath, std::unordered_map<uint64_t, uint32_t> hash64Table, bool& bSingle)
{
	std::vector<std::string> modelHashes;
	File* modelTable = new File("A67AD080", packagesPath);
	modelTable->getData();
	// Finding val
	uint32_t tableOffset = 0x40;
	uint32_t tableCount;
	memcpy((char*)&tableCount, modelTable->data + 8, 4);
	uint32_t val;
	for (int i = tableOffset; i < tableOffset + tableCount * 0x20; i += 0x20)
	{
		memcpy((char*)&val, modelTable->data + i, 4);
		if (val == apiHash)
		{
			memcpy((char*)&val, modelTable->data + i + 0x18, 4);
			if (val != 0) modelHashes = getAPIMultiHashes(val+i+0x18, modelTable, packagesPath, hash64Table);
			else
			{
				uint32_t fHash;
				memcpy((char*)&val, modelTable->data + i + 0x8, 4);
				memcpy((char*)&fHash, modelTable->data + i + 0xC, 4);
				modelHashes = getAPISingleHashes(val, fHash, packagesPath, hash64Table);
				bSingle = true;
			}
		}
	}
	delete modelTable;
	return modelHashes;
}

std::vector<std::string> getAPISingleHashes(uint32_t mHash, uint32_t fHash, std::string packagesPath, std::unordered_map<uint64_t, uint32_t> hash64Table)
{
	std::vector<std::string> h64Files = { "", "" };
	File pairTable = File("BEB2C180", packagesPath);
	pairTable.getData();
	uint32_t tableOffset = 0x30;
	uint32_t tableCount;
	memcpy((char*)&tableCount, pairTable.data + 8, 4);
	uint32_t val;
	for (int i = tableOffset; i < tableOffset + tableCount * 8; i += 8)
	{
		memcpy((char*)&val, pairTable.data + i, 4);
		if (val == mHash)
		{
			memcpy((char*)&val, pairTable.data + i + 4, 4);
			h64Files[0] = uint32ToHexStr(val);
		}
		else if (val == fHash)
		{
			memcpy((char*)&val, pairTable.data + i + 4, 4);
			h64Files[1] = uint32ToHexStr(val);
		}
		if (h64Files[0] != "" && h64Files[1] != "") break;
	}
	
	return getHashesFromH64s(h64Files, packagesPath, hash64Table);;
}

std::vector<std::string> getAPIMultiHashes(uint32_t tableOffset, File* modelTable, std::string packagesPath, std::unordered_map<uint64_t, uint32_t> hash64Table)
{
	uint32_t val1;
	uint32_t val2;
	std::vector<uint32_t> offsets;
	std::vector<std::string> h64Files;
	std::vector<std::uint32_t> pairFiles;
	uint32_t tableCount;
	memcpy((char*)&tableCount, modelTable->data + tableOffset, 4);
	tableOffset += 0x10;
	for (int i = tableOffset; i < tableOffset + tableCount * 8; i += 8)
	{
		memcpy((char*)&val1, modelTable->data + i, 4);
		val1 += i + 0x10;
		memcpy((char*)&val2, modelTable->data + val1, 4);
		val2 += val1;
		uint32_t tableCount2;
		memcpy((char*)&tableCount2, modelTable->data + val2, 4);
		val2 += 0x10;
		uint32_t val3;
		for (int j = val2; j < val2 + tableCount2 * 4; j += 4)
		{
			memcpy((char*)&val3, modelTable->data + val2, 4);
			pairFiles.push_back(val3);
		}
	}

	File pairTable = File("BEB2C180", packagesPath);
	pairTable.getData();
	uint32_t table2Offset = 0x30;
	uint32_t table2Count;
	memcpy((char*)&table2Count, pairTable.data + 8, 4);
	uint32_t val;
	for (int i = table2Offset; i < table2Offset + table2Count * 8; i += 8)
	{
		memcpy((char*)&val, pairTable.data + i, 4);
		for (auto& hash : pairFiles)
		{
			if (val == hash)
			{
				memcpy((char*)&val, pairTable.data + i + 4, 4);
				h64Files.push_back(uint32ToHexStr(val));
			}
		}
	}

	return getHashesFromH64s(h64Files, packagesPath, hash64Table);
}

std::vector<std::string> getHashesFromH64s(std::vector<std::string> h64Files, std::string packagesPath, std::unordered_map<uint64_t, uint32_t> hash64Table)
{
	std::vector<std::string> modelHashes;
	uint64_t h64;
	for (auto& f : h64Files)
	{
		if (f == "") continue;
		File file = File(f, packagesPath);
		file.getData();
		memcpy((char*)&h64, file.data + 0x10, 8);
		std::string hash = getHash64(h64, hash64Table);
		modelHashes.push_back(hash);
	}
	return modelHashes;
}
