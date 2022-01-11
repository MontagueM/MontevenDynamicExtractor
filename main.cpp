#include "main.h"
#include "dynamic.h"
#include "helpers.h"
#include "texture.h"
#include "d1map.h"

/*
Using Sarge https://mayaposch.wordpress.com/2019/03/17/parsing-command-line-arguments-in-c/
*/
int main(int argc, char** argv)
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	Dynamic dynamic = Dynamic("5b82a780", "P:/D1/packages/", eTextureFormat::DDS, "b227ad80");
	dynamic.get();
	dynamic.pack("C:/Users/monta/Downloads/phonond1maps/");
	dynamic.save("C:/Users/monta/Downloads/phonond1maps/", "test_model");
 	exit(2222);

	D1Map dm = D1Map("74449881", "P:/D1/packages/", eTextureFormat::DDS);
	dm.Get();
	dm.Extract("C:/Users/monta/Downloads/phonond1maps/", "test_map");
	exit(2313);
	Sarge sarge;

	sarge.setArgument("p", "pkgspath", "pkgs path", true);
	sarge.setArgument("o", "outputpath", "output path", true);
	sarge.setArgument("n", "filename", "output file name", true);
	sarge.setArgument("i", "inputhash", "hash of Dynamic Model Header 1", true);
	sarge.setArgument("t", "textures", "enables textures", false);
	sarge.setArgument("b", "batch", "batch with pkg ID", true);
	sarge.setArgument("a", "api", "api hash", true);
	sarge.setArgument("s", "skeloverride", "skeleton override", true);
	sarge.setArgument("c", "cbuffer", "enable cbuffer extraction", false);
	sarge.setArgument("h", "shader", "shader hash", true);
	sarge.setDescription("Destiny 2 dynamic model extractor by Monteven.");
	sarge.setUsage("MontevenDynamicExtractor");

	if (!sarge.parseArguments(argc, argv))
	{
		std::cerr << "Couldn't parse arguments..." << std::endl;
		return 1;
	}

	std::string pkgsPath;
	std::string outputPath;
	std::string fileName;
	std::string modelHash;
	bool bTextures = false;
	std::string batchPkg;
	sarge.getFlag("pkgspath", pkgsPath);
	sarge.getFlag("outputpath", outputPath);
	sarge.getFlag("filename", fileName);
	sarge.getFlag("inputhash", modelHash);

	bTextures = sarge.exists("textures");
	sarge.getFlag("batch", batchPkg);

	if (fileName == "")
	{
		fileName = modelHash;
	}

	// Checking params are valid
	if (pkgsPath == "" || outputPath == "" || (modelHash == "" && batchPkg == ""))
	{
		std::cerr << "Invalid parameters, potentially backslashes in paths or paths not given.\n";
		return 1;
	}
	else if (!std::filesystem::exists(outputPath) || !std::filesystem::exists(pkgsPath))
	{
		std::cerr << "Output path or packages path does not exist. Check they exist and try again.\n";
		return 1;
	}

	if (pkgsPath.find('\\') != std::string::npos || outputPath.find('\\') != std::string::npos)
	{
		printf("\nBackslashes in paths detected, please change to forward slashes (/).\n");
		return 1;
	}

	if (batchPkg != "")
	{
		printf("Batch flag found, exporting batch...\n");
		doBatch(pkgsPath, outputPath, batchPkg);
		printf("Batch done!\n");
		return 0;
	}

	printf("\nBeginning to extract model...\n");
	//std::string reference = getReferenceFromHash("0174", modelHash);
	Dynamic dyn(modelHash, pkgsPath, eTextureFormat::TGA, "");

	bool status = dyn.get();
	if (status)
	{
		printf("\n\nFile extraction readied...\n");
		outputPath += "/" + fileName + "/";
		dyn.pack(outputPath);
		dyn.save(outputPath, fileName);
		std::cout << "\nFile extraction complete! Saved to " << outputPath << "/" << fileName << ".fbx\n";
	}
	else
		printf("\nDynamic has no mesh data (A), skipping...\n");

	return 0;
}

void doBatch(std::string pkgsPath, std::string outputPath, std::string batchPkg)
{
	// We need to get an array of all the valid dyn1 hashes
	Package pkg(batchPkg, pkgsPath);
	std::vector<std::string> hashes = pkg.getAllFilesGivenRef("0f9c8080");
	outputPath += "/" + batchPkg + "/";
	std::cout << "\nNumber of files to batch extract: " << hashes.size() << "\n";
	for (auto& hash : hashes)
	{
		Dynamic dyn(hash, pkgsPath, eTextureFormat::None, "");
		bool status = dyn.get();
		if (status)
		{
			dyn.pack(outputPath);
			dyn.save(outputPath, hash);
			std::cout << "\nFile extraction complete! Saved to " << outputPath << "/" << hash << ".fbx\n";
		}
		else
			printf("\nDynamic has no mesh data (A), skipping...\n");
	}
}

void replaceBackslashes(std::string& path)
{
	for (int i = 0; i < path.size(); i++)
	{
		if (path[i] == '\\')
		{
			path.insert(i, 1, '\\');
			i++;
		}
	}
}
