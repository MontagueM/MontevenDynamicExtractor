#include "main.h"
#include "helpers.h"
#include "package.h"

int main()
{
	packagesPath = "I:/SteamLibrary/steamapps/common/Destiny 2/packages/";

	Package Pkg("011f", packagesPath);
	Pkg.Unpack();

	return 0;
}