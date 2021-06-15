#include "helpers.h"

std::string getReferenceFromHash(std::string hash, std::string pkgsPath)
{
	Package pkg(getPkgID(hash), pkgsPath);
	std::string reference = pkg.getEntryReference(hash);
	return reference;
}

File::File(std::string x, std::string pkgsPath)
{
	hash = x;
	packagesPath = pkgsPath;
}

int File::getData()
{
	if (pkgID == "")
	{
		pkgID = getPkgID(hash);
	}
	Package pkg(pkgID, packagesPath);
	int fileSize;
	data = pkg.getEntryData(hash, fileSize);
	if (data == nullptr) return 0;
	return fileSize;
}

std::string getPkgID(std::string hash)
{
	std::string pkgID = uint16ToHexStr(floor((hexStrToUint32(hash) - 0x80800000)/8192));
	return pkgID;
}

uint16_t getPkgID(uint32_t hash)
{
	uint16_t pkgID = floor((hash - 0x80800000) / 8192);
	return pkgID;
}

std::string getHash64(uint64_t hash64, std::unordered_map<uint64_t, uint32_t> hash64Table)
{
	std::string h64 = "";
	try
	{
		h64 = uint32ToHexStr(hash64Table[hash64]);
		if (h64 == "00000000")
			throw h64;
	}
	catch (std::string err)
	{
		std::cerr << "H64 file is out-of-date. Please delete and retry.\n";
		exit(1);
	}
	
	return h64;
}