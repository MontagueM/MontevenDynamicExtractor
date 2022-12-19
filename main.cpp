#include "main.h"
#include "dynamic.h"
#include "helpers.h"
#include "texture.h"

static void show_usage()
{
	std::cerr << "Usage: MontevenDynamicExtractorv1.8.0 -p [packages path] -o [output path] -n [file name] -i [input hash] -t -b [package ID] -a [api hash] -h [shader hash] -c"
		<< std::endl;
	std::cerr << "-t enables texture extraction\n";
	std::cerr << "-b [package ID] extracts all the dynamic models available for that package ID. -t, -i, -n are ignored\n";
	std::cerr << "-a [api hash] extracts the models paired with that given api hash if valid. -i, -b are ignored\n";
	std::cerr << "-h [api shader hash] extracts the shader that is paired with the given api hash. -i, -b, -a are ignored.\n";
	std::cerr << "-c enables cbuffer extraction";
}

/*
Using Sarge https://mayaposch.wordpress.com/2019/03/17/parsing-command-line-arguments-in-c/
*/

int main(int argc, char** argv)
{
	CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

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
		show_usage();
		return 1;
	}

	std::string pkgsPath;
	std::string outputPath;
	std::string fileName;
	std::string modelHash;
	bool bTextures = false;
	bool bCBuffer = false;
	std::string skeletonOverrideStr = "";
	int skeletonOverride = -1;
	std::string batchPkg;
	std::string apiHashStr = "";
	std::string shaderHashStr = "";
	uint32_t apiHash = 0;
	uint32_t shaderHash = 0;
	sarge.getFlag("pkgspath", pkgsPath);
	sarge.getFlag("outputpath", outputPath);
	sarge.getFlag("filename", fileName);
	sarge.getFlag("inputhash", modelHash);
	sarge.getFlag("api", apiHashStr);
	sarge.getFlag("shader", shaderHashStr);
	sarge.getFlag("skeloverride", skeletonOverrideStr);

	if (skeletonOverrideStr != "") skeletonOverride = std::stol(skeletonOverrideStr);
	if (apiHashStr != "") apiHash = std::stoul(apiHashStr);
	if (shaderHashStr != "") shaderHash = std::stoul(shaderHashStr);
	bTextures = sarge.exists("textures");
	bCBuffer = sarge.exists("cbuffer");
	sarge.getFlag("batch", batchPkg);

	if (fileName == "")
	{
		if (apiHashStr != "")
			fileName = apiHashStr;
		else if (shaderHashStr != "")
			fileName = shaderHashStr;
		else
			fileName = modelHash;
	}

	// Checking params are valid
	if (pkgsPath == "" || outputPath == "" || (modelHash == "" && batchPkg == "" && apiHash == 0 && shaderHash == 0))
	{
		std::cerr << "Invalid parameters, paths not given.\n";
		show_usage();
		return 1;
	}
	else if (!std::filesystem::exists(pkgsPath))
	{
		std::cerr << "Packages path does not exist. Check that it exists and try again.\n";
		show_usage();
		return 1;
	}
	else if (!std::filesystem::exists(outputPath))
	{
		std::filesystem::create_directories(outputPath);
	}

	if (pkgsPath.find('\\') != std::string::npos || outputPath.find('\\') != std::string::npos)
	{
		std::replace(pkgsPath.begin(), pkgsPath.end(), '\\', '/');
		std::replace(outputPath.begin(), outputPath.end(), '\\', '/');
	}

	// Check if h64 file exists, if not then generate and save
	std::unordered_map<uint64_t, uint32_t> hash64Table;
	std::ifstream f("h64");
	if (f)
	{
		hash64Table = loadH64Table();
		if (hash64Table.size() < 10000)
		{
			hash64Table = generateH64Table(pkgsPath);
			saveH64Table(hash64Table);
		}
	}
	else
	{
		hash64Table = generateH64Table(pkgsPath);
		saveH64Table(hash64Table);
	}

	if (shaderHash != 0)
	{
		printf("Shader flag found, getting shader data...\n");
		std::string savePath = outputPath + "/" + std::to_string(shaderHash);
		std::filesystem::create_directories(savePath);
		bool status = getAPIShader(shaderHash, savePath, pkgsPath, hash64Table);
		if (status)
		{
			printf("Shader rip done!");
		}
		else
		{
			printf("Shader rip failed!");
		}
		return 0;
	}

	if (apiHash != 0)
	{
		printf("API flag found, getting api models...\n");
		bool bSingle = false;
		std::vector<std::string> hashes = getAPIModelHashes(apiHash, pkgsPath, hash64Table, bSingle);

		if (!hashes.size())
		{
			printf("API hash has no valid data, try another hash for the same item.\n");
			return 1;
		}

		printf("exporting api model...\n");
		for (int i = 0; i < hashes.size(); i++)
		{
			std::string savePath = outputPath;
			std::string fName = fileName;
			if (hashes.size() == 2 && bSingle)
			{
				if (i)
					fName += "_f";
				else
					fName += "_m";
			}
			savePath += "/" + fName + "/";
			std::string h = hashes[i];
			if (h == "ffffffff") continue;
			Dynamic dyn(h, hash64Table, pkgsPath, bTextures, skeletonOverride);
			if (dyn.get())
			{
				printf("\n\nFile extraction readied...\n");
				dyn.pack(savePath, bCBuffer);
				dyn.save(savePath, fName + "_" + h);
			}
			else
				printf("\nDynamic has no mesh data (A), skipping...\n");
		}
		printf("API rip done!");
		return 0;
	}

	if (batchPkg != "")
	{
		printf("Batch flag found, exporting batch...\n");
		doBatch(pkgsPath, outputPath, batchPkg, hash64Table, skeletonOverride);
		printf("Batch done!\n");
		return 0;
	}

	printf("\nBeginning to extract model...\n");
	//std::string reference = getReferenceFromHash("0174", modelHash);
	Dynamic dyn(modelHash, hash64Table, pkgsPath, bTextures, skeletonOverride);

	bool status = dyn.get();
	if (status)
	{
		printf("\n\nFile extraction readied...\n");
		outputPath += "/" + fileName + "/";
		dyn.pack(outputPath, bCBuffer);
		dyn.save(outputPath, fileName);
		std::cout << "\nFile extraction complete! Saved to " << outputPath << "/" << fileName << ".fbx\n";
	}
	else
		printf("\nDynamic has no mesh data (A), skipping...\n");

	return 0;
}

void doBatch(std::string pkgsPath, std::string outputPath, std::string batchPkg, std::unordered_map<uint64_t, uint32_t> hash64Table, int skeletonOverride)
{
	// We need to get an array of all the valid dyn1 hashes
	Package pkg(batchPkg, pkgsPath);
	std::vector<std::string> hashes = pkg.getAllFilesGivenRef("d89a8080");
	outputPath += "/" + batchPkg + "/";
	std::cout << "\nNumber of files to batch extract: " << hashes.size() << "\n";
	for (auto& hash : hashes)
	{
		Dynamic dyn(hash, hash64Table, pkgsPath, false, skeletonOverride);
		bool status = dyn.get();
		if (status)
		{
			dyn.pack(outputPath, false);
			dyn.save(outputPath, hash);
			std::cout << "\nFile extraction complete! Saved to " << outputPath << "/" << hash << ".fbx\n";
		}
		else
			printf("\nDynamic has no mesh data (A), skipping...\n");
	}
}