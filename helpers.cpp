#include "helpers.h"

File::File(std::string x, std::string pkgsPath)
{
	hash = x;
	packagesPath = pkgsPath;
}

int File::getData()
{
	// check if (string to int) 
	int hashInt = hexStrToUint32(hash);
	if (hashInt < 0x80a00000 || hashInt > 0x81ffffff)
	{
		std::cout << "Invalid hash: " << hash << std::endl;
		return 0;
	}


	if (pkgID == "")
	{
		pkgID = getPkgID(hash);
	}
	Package pkg(pkgID, packagesPath);
	int fileSize;
	data = pkg.getEntryData(hash, fileSize);
	if (data == nullptr || sizeof(data) == 0) return 0;
	return fileSize;
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