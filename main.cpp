#include "main.h"
#include "dynamic.h"
#include "helpers.h"

int main()
{
	std::string modelHash = "B0E6B080";

	DynamicMesh* mesh = new DynamicMesh();
	DynamicSubmesh* submesh = new DynamicSubmesh();

	//std::string reference = getReferenceFromHash("0174", modelHash);
	Dynamic dyn(modelHash);
	dyn.get();
	dyn.pack("I:/dynamic_models/cpp/");
	dyn.save("I:/dynamic_models/cpp/", "test");
	return 0;
}