#include "main.h"
#include "helpers.h"

int main()
{
	std::string packagesPath = "I:/SteamLibrary/steamapps/common/Destiny 2/packages/";
	std::string modelHash = "B5FFB080";

	std::string reference = getReferenceFromHash(packagesPath, "0187", modelHash);
	return 0;
}