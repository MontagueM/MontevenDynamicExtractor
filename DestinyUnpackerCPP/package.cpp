#include "package.h"


const static unsigned char AES_KEY_0[16] =
{
	0xD6, 0x2A, 0xB2, 0xC1, 0x0C, 0xC0, 0x1B, 0xC5, 0x35, 0xDB, 0x7B, 0x86, 0x55, 0xC7, 0xDC, 0x3B,
};

const static unsigned char AES_KEY_1[16] =
{
	0x3A, 0x4A, 0x5D, 0x36, 0x73, 0xA6, 0x60, 0x58, 0x7E, 0x63, 0xE6, 0x76, 0xE4, 0x08, 0x92, 0xB5,
};

const int BLOCK_SIZE = 0x40000;

Package::Package(std::string packageID, std::string pkgsPath)
{
	packagesPath = pkgsPath;
	if (!std::filesystem::exists(packagesPath))
	{
		printf("Package path given is invalid!");
		exit(1);
	}
	packagePath = getLatestPatchIDPath(packageID);
}

std::string Package::getLatestPatchIDPath(std::string packageID)
{
	std::string fullPath = "";
	uint16_t patchID;
	int largestPatchID = -1;
	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(packagesPath))
	{
		fullPath = entry.path().u8string();
		if (fullPath.find(packageID) != std::string::npos)
		{
			patchID = std::stoi(fullPath.substr(fullPath.size() - 5, 1));
			if (patchID > largestPatchID) largestPatchID = patchID;
			std::replace(fullPath.begin(), fullPath.end(), '\\', '/');
			packageName = fullPath.substr(0, fullPath.size() - 6);
			packageName = packageName.substr(packageName.find_last_of('/'));
		}
	}
	// Some strings are not covered, such as the bootstrap set so we need to do pkg checks
	if (largestPatchID == -1)
	{
		FILE* patchPkg = nullptr;
		uint16_t pkgID;
		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(packagesPath))
		{
			fullPath = entry.path().u8string();


			errno_t status = fopen_s(&patchPkg, fullPath.c_str(), "rb");
			if (patchPkg == nullptr || status) exit(67);
			fseek(patchPkg, 0x10, SEEK_SET);
			fread((char*)&pkgID, 1, 2, patchPkg);

			if (packageID == uint16ToHexStr(pkgID))
			{
				fseek(patchPkg, 0x30, SEEK_SET);
				fread((char*)&patchID, 1, 2, patchPkg);
				if (patchID > largestPatchID) largestPatchID = patchID;
				std::replace(fullPath.begin(), fullPath.end(), '\\', '/');
				packageName = fullPath.substr(0, fullPath.size() - 6);
				packageName = packageName.substr(packageName.find_last_of('/'));
			}
			fclose(patchPkg);
		}
	}

	return packagesPath + "/" + packageName + "_" + std::to_string(largestPatchID) + ".pkg";
}

bool Package::readHeader()
{
	// Package data
	auto status = fopen_s(&pkgFile, packagePath.c_str(), "rb");
	if (status != 0)
	{
		return false;
	}
	fseek(pkgFile, 0x10, SEEK_SET);
	fread((char*)&header.pkgID, 1, 2, pkgFile);

	fseek(pkgFile, 0x30, SEEK_SET);
	fread((char*)&header.patchID, 1, 2, pkgFile);

	// Entry Table
	fseek(pkgFile, 0x44, SEEK_SET);
	fread((char*)&header.entryTableOffset, 1, 4, pkgFile);

	fseek(pkgFile, 0x60, SEEK_SET);
	fread((char*)&header.entryTableSize, 1, 4, pkgFile);

	// Block Table

	fseek(pkgFile, 0x68, SEEK_SET);
	fread((char*)&header.blockTableSize, 1, 4, pkgFile);
	fread((char*)&header.blockTableOffset, 1, 4, pkgFile);

	// Hash64 Table
	fseek(pkgFile, 0xB8, SEEK_SET);
	fread((char*)&header.hash64TableSize, 1, 4, pkgFile);
	fread((char*)&header.hash64TableOffset, 1, 4, pkgFile);
	header.hash64TableOffset += 64; // relative offset
	return true;
}

void Package::getEntryTable()
{
	for (uint32_t i = header.entryTableOffset; i < header.entryTableOffset + header.entryTableSize * 16; i += 16)
	{
		Entry entry;

		// EntryA
		uint32_t entryA;
		fseek(pkgFile, i, SEEK_SET);
		fread((char*)&entryA, 1, 4, pkgFile);
		entry.reference = uint32ToHexStr(entryA);

		// EntryB
		uint32_t entryB;
		fread((char*)&entryB, 1, 4, pkgFile);
		entry.numType = (entryB >> 9) & 0x7F;
		entry.numSubType = (entryB >> 6) & 0x7;

		// EntryC
		uint32_t entryC;
		fread((char*)&entryC, 1, 4, pkgFile);
		entry.startingBlock = entryC & 0x3FFF;
		entry.startingBlockOffset = ((entryC >> 14) & 0x3FFF) << 4;

		// EntryD
		uint32_t entryD;
		fread((char*)&entryD, 1, 4, pkgFile);
		entry.fileSize = (entryD & 0x3FFFFFF) << 4 | (entryC >> 28) & 0xF;

		entries.push_back(entry);
	}
}

void Package::getBlockTable()
{
	for (uint32_t i = header.blockTableOffset; i < header.blockTableOffset + header.blockTableSize * 48; i += 48)
	{
		Block block = { 0, 0, 0, 0, 0 };
		fseek(pkgFile, i, SEEK_SET);
		fread((char*)&block.offset, 1, 4, pkgFile);
		fread((char*)&block.size, 1, 4, pkgFile);
		fread((char*)&block.patchID, 1, 2, pkgFile);
		fread((char*)&block.bitFlag, 1, 2, pkgFile);
		fseek(pkgFile, i + 0x20, SEEK_SET);
		fread((char*)&block.gcmTag, 16, 1, pkgFile);
		blocks.push_back(block);
	}
}

void Package::modifyNonce()
{
	// Nonce
	nonce[0] ^= (header.pkgID >> 8) & 0xFF;
	nonce[11] ^= header.pkgID & 0xFF;
}

void Package::extractFiles()
{
	std::vector<std::string> pkgPatchStreamPaths;
	std::string outputPath = CUSTOM_DIR + "/output/" + uint16ToHexStr(header.pkgID);
	std::filesystem::create_directories(outputPath);
	// Initialising the required file streams
	for (int i = 0; i <= header.patchID; i++)
	{
		std::string pkgPatchPath = packagePath;
		pkgPatchPath[pkgPatchPath.size() - 5] = char(i + 48);
		pkgPatchStreamPaths.push_back(pkgPatchPath);
		std::cout << pkgPatchPath << "\n";
	}
	// Extracting each entry to a file
	for (int i = 0; i < entries.size(); i++)
	{
		Entry entry = entries[i];
		int currentBlockID = entry.startingBlock;
		int blockCount = floor((entry.startingBlockOffset + entry.fileSize - 1) / BLOCK_SIZE);
		if (entry.fileSize == 0) blockCount = 0; // Stupid check for weird C++ floor behaviour
		int lastBlockID = currentBlockID + blockCount;
		unsigned char* fileBuffer = new unsigned char[entry.fileSize];
		int currentBufferOffset = 0;
		while (currentBlockID <= lastBlockID)
		{
			Block currentBlock = blocks[currentBlockID];

			FILE* pFile;
			fopen_s(&pFile, pkgPatchStreamPaths[currentBlock.patchID].c_str(), "rb");
			fseek(pFile, currentBlock.offset, SEEK_SET);
			unsigned char* blockBuffer = new unsigned char[currentBlock.size];
			size_t result;
			result = fread(blockBuffer, 1, currentBlock.size, pFile);
			if (result != currentBlock.size) { fputs("Reading error", stderr); exit(3); }

			unsigned char* decryptBuffer = new unsigned char[currentBlock.size];
			unsigned char* decompBuffer = new unsigned char[BLOCK_SIZE];

			if (currentBlock.bitFlag & 0x2)
				decryptBlock(currentBlock, blockBuffer, decryptBuffer);
			else
				decryptBuffer = blockBuffer;

			if (currentBlock.bitFlag & 0x1)
				decompressBlock(currentBlock, decryptBuffer, decompBuffer);
			else
				decompBuffer = decryptBuffer;

			if (currentBlockID == entry.startingBlock)
			{
				size_t cpySize;
				if (currentBlockID == lastBlockID)
					cpySize = entry.fileSize;
				else
					cpySize = BLOCK_SIZE - entry.startingBlockOffset;
				memcpy(fileBuffer, decompBuffer + entry.startingBlockOffset, cpySize);
				currentBufferOffset += cpySize;
			}
			else if (currentBlockID == lastBlockID)
			{
				memcpy(fileBuffer + currentBufferOffset, decompBuffer, entry.fileSize - currentBufferOffset);
			}
			else
			{
				memcpy(fileBuffer + currentBufferOffset, decompBuffer, BLOCK_SIZE);
				currentBufferOffset += BLOCK_SIZE;
			}

			fclose(pFile);
			currentBlockID++;
			delete[] decompBuffer;
		}

		FILE* oFile;
		std::string name = outputPath + "/" + uint16ToHexStr(header.pkgID) + "-" + uint16ToHexStr(i) + ".bin";
		fopen_s(&oFile, name.c_str(), "wb");
		fwrite(fileBuffer, entry.fileSize, 1, oFile);
		fclose(oFile);
		delete[] fileBuffer;
	}
}

// Bcrypt decryption implementation largely from Sir Kane's SourcePublic_v2.cpp, very mysterious
void Package::decryptBlock(Block block, unsigned char* blockBuffer, unsigned char*& decryptBuffer)
{
	BCRYPT_ALG_HANDLE hAesAlg;
	NTSTATUS status;
	status = BCryptOpenAlgorithmProvider(&hAesAlg, BCRYPT_AES_ALGORITHM, nullptr, 0);
	status = BCryptSetProperty(hAesAlg, BCRYPT_CHAINING_MODE, (PUCHAR)BCRYPT_CHAIN_MODE_GCM,
		sizeof(BCRYPT_CHAIN_MODE_GCM), 0);

	alignas(alignof(BCRYPT_KEY_DATA_BLOB_HEADER)) unsigned char keyData[sizeof(BCRYPT_KEY_DATA_BLOB_HEADER) + 16];
	BCRYPT_KEY_DATA_BLOB_HEADER* pHeader = (BCRYPT_KEY_DATA_BLOB_HEADER*)keyData;
	pHeader->dwMagic = BCRYPT_KEY_DATA_BLOB_MAGIC;
	pHeader->dwVersion = BCRYPT_KEY_DATA_BLOB_VERSION1;
	pHeader->cbKeyData = 16;
	memcpy(pHeader + 1, block.bitFlag & 0x4 ? AES_KEY_1 : AES_KEY_0, 16);
	BCRYPT_KEY_HANDLE hAesKey;

	status = BCryptImportKey(hAesAlg, nullptr, BCRYPT_KEY_DATA_BLOB, &hAesKey, nullptr, 0, keyData, sizeof(keyData), 0);
	ULONG decryptionResult;
	BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO cipherModeInfo;

	BCRYPT_INIT_AUTH_MODE_INFO(cipherModeInfo);

	cipherModeInfo.pbTag = (PUCHAR)block.gcmTag;
	cipherModeInfo.cbTag = 0x10;
	cipherModeInfo.pbNonce = nonce;
	cipherModeInfo.cbNonce = sizeof(nonce);

	status = BCryptDecrypt(hAesKey, (PUCHAR)blockBuffer, (ULONG)block.size, &cipherModeInfo, nullptr, 0,
		(PUCHAR)decryptBuffer, (ULONG)block.size, &decryptionResult, 0);
	if (status < 0)// && status != -1073700862)
		printf("\nbcrypt decryption failed!");
	BCryptDestroyKey(hAesKey);
	BCryptCloseAlgorithmProvider(hAesAlg, 0);

	delete[] blockBuffer;
}

void Package::decompressBlock(Block block, unsigned char* decryptBuffer, unsigned char*& decompBuffer)
{
	int64_t result = ((OodleLZ64_DecompressDef)OodleLZ_Decompress)(decryptBuffer, block.size, decompBuffer, BLOCK_SIZE, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, 3);
	if (result <= 0)
		auto a = 0;
	delete[] decryptBuffer;
}

bool Package::initOodle()
{
	hOodleDll = LoadLibrary(L"oo2core_9_win64.dll");
	if (hOodleDll == nullptr) {
		return false;
	}
	OodleLZ_Decompress = (int64_t)GetProcAddress(hOodleDll, "OodleLZ_Decompress");
	if (!OodleLZ_Decompress) printf("Failed to find Oodle compress/decompress functions in DLL!");
	return true;
}

bool Package::Unpack()
{
	readHeader();
	if (!initOodle())
	{
		printf("\nFailed to initialise oodle");
		return 1;
	}
	modifyNonce();
	getEntryTable();
	getBlockTable();
	fclose(pkgFile);
	extractFiles();
	return 0;
}

// Most efficient route to getting a single entry's reference
std::string Package::getEntryReference(std::string hash)
{
	// Entry index
	uint32_t id = hexStrToUint32(hash) % 8192;

	// Entry offset
	uint32_t entryTableOffset;
	auto status = fopen_s(&pkgFile, packagePath.c_str(), "rb");
	if (status != 0)
	{
		printf("\nFailed to initialise pkg file, exiting...\n");
		std::cerr << hash << " " << packagePath.c_str() << std::endl << packagePath << std::endl << status << std::endl;
		return "";
		//exit(status);
	}
	fseek(pkgFile, 0x44, SEEK_SET);
	fread((char*)&entryTableOffset, 1, 4, pkgFile);

	// Getting reference
	uint32_t entryA;
	fseek(pkgFile, entryTableOffset + id * 16, SEEK_SET);
	fread((char*)&entryA, 1, 4, pkgFile);
	std::string reference = uint32ToHexStr(entryA);
	fclose(pkgFile);
	return reference;
}

uint8_t Package::getEntryTypes(std::string hash, uint8_t &subType)
{
	// Entry index
	uint32_t id = hexStrToUint32(hash) % 8192;

	// Entry offset
	uint32_t entryTableOffset;
	auto status = fopen_s(&pkgFile, packagePath.c_str(), "rb");
	if (status != 0)
	{
		printf("\nFailed to initialise pkg file, exiting...\n");
		std::cerr << hash << std::endl << packagePath;
		return -1;
		//exit(1);
	}
	fseek(pkgFile, 0x44, SEEK_SET);
	fread((char*)&entryTableOffset, 1, 4, pkgFile);

	// Getting reference
	// EntryB
	uint32_t entryB;
	fseek(pkgFile, entryTableOffset + id * 16 + 4, SEEK_SET);
	fread((char*)&entryB, 1, 4, pkgFile);
	uint8_t type = (entryB >> 9) & 0x7F;
	subType = (entryB >> 6) & 0x7;
	fclose(pkgFile);
	return type;
}

// This gets the minimum required data to pull out a single file from the game
unsigned char* Package::getEntryData(std::string hash, int& fileSize)
{
	// Entry index
	uint32_t id = hexStrToUint32(hash) % 8192;

	// Header data
	if (header.pkgID == 0)
	{
		bool status = readHeader();
		if (!status) return nullptr;
	}

	if (id >= header.entryTableSize) return nullptr;

	Entry entry;

	// EntryC
	uint32_t entryC;
	fseek(pkgFile, header.entryTableOffset + id * 16 + 8, SEEK_SET);
	fread((char*)&entryC, 1, 4, pkgFile);
	entry.startingBlock = entryC & 0x3FFF;
	entry.startingBlockOffset = ((entryC >> 14) & 0x3FFF) << 4;

	// EntryD
	uint32_t entryD;
	fread((char*)&entryD, 1, 4, pkgFile);
	entry.fileSize = (entryD & 0x3FFFFFF) << 4 | (entryC >> 28) & 0xF;
	fileSize = entry.fileSize;

	// Getting data to return
	if (!initOodle())
	{
		printf("\nFailed to initialise oodle, exiting...");
		exit(1);
	}
	modifyNonce();

	unsigned char* buffer = getBufferFromEntry(entry);
	fclose(pkgFile);
	return buffer;
}

std::unordered_map<uint64_t, uint32_t> generateH64Table(std::string packagesPath)
{
	std::set<std::string> pkgIDs;
	std::unordered_map<uint64_t, uint32_t> hash64Table;

	std::string path;
	int status;
	std::string fullPath;
	std::string reducedPath;
	uint16_t pkgIDBytes;
	std::string pkgID;
	FILE* pkgFile;
	// Getting all packages
	for (const auto& entry : std::filesystem::directory_iterator(packagesPath))
	{
		path = entry.path().u8string();
		status = fopen_s(&pkgFile, path.c_str(), "rb");
		if (status)
		{
			std::cerr << "FAILED GETTING PACKAGES FOR H64 ERR1515";
			exit(status);
		}
		fseek(pkgFile, 0x10, SEEK_SET);
		fread((char*)&pkgIDBytes, 1, 2, pkgFile);
		pkgID = uint16ToHexStr(pkgIDBytes);
		pkgIDs.insert(pkgID);
		fclose(pkgFile);
	}
	for (auto& pkgID : pkgIDs)
	{
		Package pkg = Package(pkgID, packagesPath);
		status = fopen_s(&pkgFile, pkg.packagePath.c_str(), "rb");
		if (status)
		{
			std::cerr << "FAILED GETTING PACKAGES FOR H64 ERR5632";
			exit(status);
		}
		// Hash64 Table
		uint32_t hash64TableCount;
		uint32_t hash64TableOffset;
		fseek(pkgFile, 0xB8, SEEK_SET);
		fread((char*)&hash64TableCount, 1, 4, pkgFile);
		if (!hash64TableCount) continue;
		fread((char*)&hash64TableOffset, 1, 4, pkgFile);
		hash64TableOffset += 64 + 0x10;

		for (int i = hash64TableOffset; i < hash64TableOffset + hash64TableCount * 0x10; i += 0x10)
		{
			uint64_t h64Val;
			fseek(pkgFile, i, SEEK_SET);
			fread((char*)&h64Val, 1, 8, pkgFile);
			uint32_t hVal;
			fread((char*)&hVal, 1, 4, pkgFile);
			hash64Table[h64Val] = hVal;
		}
		fclose(pkgFile);
	}
	return hash64Table;
}

bool saveH64Table(std::unordered_map<uint64_t, uint32_t> hash64Table)
{
	FILE* file;
	int status = fopen_s(&file, "h64", "wb");
	if (status)
	{
		std::cerr << "FAILED WRITING H64 ERR5157";
		exit(status);
	}
	if (file == NULL) return false;
	for (auto& element : hash64Table)
	{
		fwrite(&element.first, 8, 1, file);
		fwrite(&element.second, 4, 1, file);
	}
	fclose(file);
	return true;
}

std::unordered_map<uint64_t, uint32_t> loadH64Table()
{
	std::unordered_map<uint64_t, uint32_t> hash64Table;
	FILE* file;
	int status = fopen_s(&file, "h64", "rb");
	if (status)
	{
		std::cerr << "FAILED READING H64 ERR1231";
		exit(status);
	}
	uint64_t h64Val;
	uint32_t hVal;
	fread(&h64Val, 8, 1, file);
	size_t read = fread(&hVal, 4, 1, file);
	hash64Table[h64Val] = hVal;
	while (read)
	{
		fread(&h64Val, 8, 1, file);
		read = fread(&hVal, 4, 1, file);
		hash64Table[h64Val] = hVal;
	}
	fclose(file);
	return hash64Table;
}

// For batch extraction
std::vector<std::string> Package::getAllFilesGivenRef(std::string reference)
{
	//uint32_t ref = hexStrToUint32(reference);
	std::vector<std::string> hashes;

	// Header data
	bool status = readHeader();
	if (!status) return std::vector<std::string>();

	getEntryTable();
	for (int i = 0; i < entries.size(); i++)
	{
		Entry entry = entries[i];
		if (entry.reference == reference)
		{
			uint32_t a = header.pkgID * 8192;
			uint32_t b = a + i + 2155872256;
			hashes.push_back(uint32ToHexStr(b));
		}
	}

	return hashes;
}

unsigned char* Package::getBufferFromEntry(Entry entry)
{
	if (!entry.fileSize) return nullptr;
	int blockCount = floor((entry.startingBlockOffset + entry.fileSize - 1) / BLOCK_SIZE);

	// Getting required block data
	for (uint32_t i = header.blockTableOffset + entry.startingBlock * 48; i <= header.blockTableOffset + entry.startingBlock * 48 + blockCount * 48; i += 48)
	{
		Block block = { 0, 0, 0, 0, 0 };
		fseek(pkgFile, i, SEEK_SET);
		fread((char*)&block.offset, 1, 4, pkgFile);
		fread((char*)&block.size, 1, 4, pkgFile);
		fread((char*)&block.patchID, 1, 2, pkgFile);
		fread((char*)&block.bitFlag, 1, 2, pkgFile);
		fseek(pkgFile, i + 0x20, SEEK_SET);
		fread((char*)&block.gcmTag, 16, 1, pkgFile);
		blocks.push_back(block);
	}

	unsigned char* fileBuffer = new unsigned char[entry.fileSize];
	int currentBufferOffset = 0;
	int currentBlockID = 0;
	for (const Block& currentBlock : blocks) // & here is good as it captures by const reference, cheaper than by value
	{
		packagePath[packagePath.size() - 5] = currentBlock.patchID + 48;
		FILE* pFile;
		int status = fopen_s(&pFile, packagePath.c_str(), "rb");
		while (status)
		{
			status = fopen_s(&pFile, packagePath.c_str(), "rb");
			//std::cerr << "FAILED GETTING PACKAGE FOR BLOCK EXTRACT ERR9532 {PKG: " << packagePath << "}" << std::endl;
			//std::cerr << status << std::endl;
			//fclose(pFile);
			//return new unsigned char[0];
			////exit(status);
		}
		fseek(pFile, currentBlock.offset, SEEK_SET);
		unsigned char* blockBuffer = new unsigned char[currentBlock.size];
		size_t result;
		result = fread(blockBuffer, 1, currentBlock.size, pFile);
		if (result != currentBlock.size) { fputs("Reading error", stderr); exit(3); }

		unsigned char* decryptBuffer = new unsigned char[currentBlock.size];
		unsigned char* decompBuffer = new unsigned char[BLOCK_SIZE];

		if (currentBlock.bitFlag & 0x2)
			decryptBlock(currentBlock, blockBuffer, decryptBuffer);
		else
			decryptBuffer = blockBuffer;

		if (currentBlock.bitFlag & 0x1)
			decompressBlock(currentBlock, decryptBuffer, decompBuffer);
		else
			decompBuffer = decryptBuffer;

		if (currentBlockID == 0)
		{
			size_t cpySize;
			if (currentBlockID == blockCount)
				cpySize = entry.fileSize;
			else
				cpySize = BLOCK_SIZE - entry.startingBlockOffset;
			memcpy(fileBuffer, decompBuffer + entry.startingBlockOffset, cpySize);
			currentBufferOffset += cpySize;
		}
		else if (currentBlockID == blockCount)
		{
			memcpy(fileBuffer + currentBufferOffset, decompBuffer, entry.fileSize - currentBufferOffset);
		}
		else
		{
			memcpy(fileBuffer + currentBufferOffset, decompBuffer, BLOCK_SIZE);
			currentBufferOffset += BLOCK_SIZE;
		}

		fclose(pFile);
		currentBlockID++;
		delete[] decompBuffer;
	}
	blocks.clear();
	return fileBuffer;
}
