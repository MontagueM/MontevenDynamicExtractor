#include "helpers.h"

std::string getReferenceFromHash(std::string hash)
{
	Package pkg(getPkgID(hash));
	std::string reference = pkg.getEntryReference(hash);
	return reference;
}

File::File(std::string x)
{
	hash = x;
}

int File::getData()
{
	if (pkgID == "")
	{
		pkgID = getPkgID();
	}
	Package pkg(pkgID);
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