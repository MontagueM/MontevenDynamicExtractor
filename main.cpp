#include "main.h"
#include "dynamic.h"
#include "helpers.h"
#include "texture.h"


static void show_usage()
{
	std::cerr << "Usage: DestinyDynamicExtractor.exe -p [packages path] -o [output path] -n [file name] -i [input hash]"
		<< std::endl;
}

/*
Format to run exe is

DestinyDynamicExtractor.exe -p [packages path] -o [output path] -n [file name] -i [input hash]

Using Sarge https://mayaposch.wordpress.com/2019/03/17/parsing-command-line-arguments-in-c/
*/
int main(int argc, char** argv)
{
	if (true)
	{
		std::string pkgsPath = "I:/SteamLibrary/steamapps/common/Destiny 2/packages/";
		std::string h64 = getHash64("0000CE9FABB437ED", pkgsPath);
		return 0;

		Texture tex = Texture("8425CF80", pkgsPath);
		tex.tex2DDS("I:/test_out/8425CF80.dds");
		return 0;
	}

	// Debug
	if (true)
	{
		std::string pkgsPath = "I:/SteamLibrary/steamapps/common/Destiny 2/packages/";
		std::string outputPath = "I:/dynamic_models/cpp/";
		std::string fileName = "tt";
		std::string modelHash = "B0E6B080";
		DynamicMesh* mesh = new DynamicMesh();
		DynamicSubmesh* submesh = new DynamicSubmesh();
		printf("\nBeginning to extract model...\n");
		//std::string reference = getReferenceFromHash("0174", modelHash);
		Dynamic dyn(modelHash, pkgsPath);
		dyn.get();
		printf("\n\nFile extraction readied...\n");
		dyn.pack(outputPath);
		dyn.save(outputPath, fileName);
		std::cout << "\nFile extraction complete! Saved to" << outputPath << "/" << fileName << ".fbx\n";
		return 0;
	}

	if (argc != 9)
	{
		show_usage();
		std::cout << argc;
		return 1;
	}

	Sarge sarge;

	sarge.setArgument("p", "pkgspath", "pkgs path", true);
	sarge.setArgument("o", "outputpath", "output path", true);
	sarge.setArgument("n", "filename", "output file name", true);
	sarge.setArgument("i", "inputhash", "hash of Dynamic Model Header 1", true);
	sarge.setDescription("Destiny 2 dynamic model extractor by Monteven.");
	sarge.setUsage("DestinyDynamicExtractor ");

	if (!sarge.parseArguments(argc, argv))
	{
		std::cerr << "Couldn't parse arguments..." << std::endl;
		return 1;
	}
	std::cout << "Number of flags found: " << sarge.flagCount() << std::endl;
	std::string pkgsPath;
	std::string outputPath;
	std::string fileName;
	std::string modelHash;
	sarge.getFlag("pkgspath", pkgsPath);
	sarge.getFlag("outputpath", outputPath);
	sarge.getFlag("filename", fileName);
	sarge.getFlag("inputhash", modelHash);

	//std::string modelHash = "B0E6B080";
	std::string password;
	std::cout << "if you're not jud go away thanks pswd: ";
	std::cin >> password;
	if (password != "warlock")
	{
		printf("Wrong password");
		return 69;
	}

	DynamicMesh* mesh = new DynamicMesh();
	DynamicSubmesh* submesh = new DynamicSubmesh();
	printf("\nBeginning to extract model...\n");
	//std::string reference = getReferenceFromHash("0174", modelHash);
	Dynamic dyn(modelHash, pkgsPath);
	dyn.get();
	printf("\n\nFile extraction readied...\n");
	dyn.pack(outputPath);
	dyn.save(outputPath, fileName);
	std::cout << "\nFile extraction complete! Saved to" << outputPath << "/" << fileName << ".fbx\n";
	return 0;
}