#include "helpers.h"

std::string getReferenceFromHash(std::string packagesPath, std::string pkgID, std::string hash)
{
	Package Pkg(packagesPath, pkgID);
	std::string reference = Pkg.getEntryReference(hash);
	return reference;
}

File::File(std::string x)
{
	id = x;
}

unsigned char* File::getData()
{
	if (pkgName == "")
	{
		getPkgID();
	}
	return nullptr;
}

std::string File::getPkgID()
{
	std::string pkgID = uint32ToHexStr(floor((hexStrToUint32(id) - 0x80800000)/8192));
	return pkgName;
}