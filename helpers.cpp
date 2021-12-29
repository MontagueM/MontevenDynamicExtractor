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
	if ((hash.substr(hash.length() - 2) != "80" && hash.substr(hash.length() - 2) != "81") || hash.substr(hash.length() - 4) == "8080") return 0;

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

std::string getPkgID(std::string hash)
{
	std::string pkgID = uint16ToHexStr(floor((hexStrToUint32(hash) - 0x80800000) / 8192));
	return pkgID;
}

uint16_t getPkgID(uint32_t hash)
{
	uint16_t pkgID = floor((hash - 0x80800000) / 8192);
	return pkgID;
}