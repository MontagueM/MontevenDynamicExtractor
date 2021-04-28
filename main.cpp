#include "main.h"
#include "helpers.h"

int main()
{
	std::string modelHash = "B0E6B080";

	//std::string reference = getReferenceFromHash("0174", modelHash);
	Dynamic dyn(modelHash);
	dyn.get();
	return 0;
}