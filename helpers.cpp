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

std::string getHash64(std::string hash64, std::string pkgsPath)
{
	//return "";
	std::string h64 = getH64(hash64, pkgsPath);
	return h64;
}