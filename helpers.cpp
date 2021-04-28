#include "helpers.h"

std::string getReferenceFromHash(std::string pkgID, std::string hash)
{
	Package pkg(pkgID);
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
		getPkgID();
	}
	Package pkg(pkgID);
	int fileSize;
	data = pkg.getEntryData(hash, fileSize);
	if (data == nullptr) return 0;
	return fileSize;
}

std::string File::getPkgID()
{
	pkgID = uint16ToHexStr(floor((hexStrToUint32(hash) - 0x80800000)/8192));
	return pkgID;
}