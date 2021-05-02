#include "main.h"
#include "dynamic.h"
#include "helpers.h"
#include "texture.h"


static void show_usage()
{
	std::cerr << "Usage: DestinyDynamicExtractor -p [packages path] -o [output path] -n [file name] -i [input hash] -t -b [package ID]"
		<< std::endl;
	std::cerr << "-t enables texture extraction\n";
	std::cerr << "-b [package ID] extracts all the dynamic models available for that package ID";
}

/*
Format to run exe is

DestinyDynamicExtractor.exe -p [packages path] -o [output path] -n [file name] -i [input hash]

Using Sarge https://mayaposch.wordpress.com/2019/03/17/parsing-command-line-arguments-in-c/
*/
int main(int argc, char** argv)
{
	if (false)
	{
		std::string pkgsPath = "I:/SteamLibrary/steamapps/common/Destiny 2/packages/";
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
		std::string outputPath = "I:/dynamic_models/cpp/";
		printf("Batch flag found, exporting batch...");
		doBatch(pkgsPath, outputPath, "011c", hash64Table);
		printf("Batch done!");
		return 0;
	}

	// Debug
	if (false)
	{
		std::string pkgsPath = "I:/SteamLibrary/steamapps/common/Destiny 2/packages/";
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
		std::string outputPath = "I:/dynamic_models/cpp/";
		std::string fileName = "whateva";
		std::string modelHash = "B0E6B080";
		outputPath += "/" + fileName + "/";

		printf("\nBeginning to extract model...\n");
		//std::string reference = getReferenceFromHash("0174", modelHash);
		Dynamic dyn(modelHash, hash64Table, pkgsPath, true);
		dyn.get();
		printf("\n\nFile extraction readied...\n");
		dyn.pack(outputPath);
		dyn.save(outputPath, fileName);
		std::cout << "\nFile extraction complete! Saved to " << outputPath << "/" << fileName << ".fbx\n";
		return 0;
	}

	//std::string modelHash = "B0E6B080";
	std::string password;
	std::cout << "if you're not jud go away thanks pswd: ";
	std::cin >> password;
	if (password != "warlock")
	{
		printf("Wrong password");
		exit(1);
	}

	Sarge sarge;

	sarge.setArgument("p", "pkgspath", "pkgs path", true);
	sarge.setArgument("o", "outputpath", "output path", true);
	sarge.setArgument("n", "filename", "output file name", true);
	sarge.setArgument("i", "inputhash", "hash of Dynamic Model Header 1", true);
	sarge.setArgument("t", "textures", "enables textures", false);
	sarge.setArgument("b", "batch", "batch with pkg ID", true);
	sarge.setArgument("h", "help", "help shows arguments", false);
	sarge.setDescription("Destiny 2 dynamic model extractor by Monteven.");
	sarge.setUsage("DestinyDynamicExtractor ");

	if (!sarge.parseArguments(argc, argv))
	{
		std::cerr << "Couldn't parse arguments..." << std::endl;
		show_usage();
		return 1;
	}

	if (sarge.exists("help"))
	{
		show_usage();
		return 0;
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

	// Checking params are valid
	if (pkgsPath == "" || modelHash == "")
	{
		std::cerr << "Invalid parameters";
		show_usage();
		exit(1);
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

	if (batchPkg != "")
	{
		printf("Batch flag found, exporting batch...");
		doBatch(pkgsPath, outputPath, batchPkg, hash64Table);
		printf("Batch done!");
		return 0;
	}

	if (fileName == "") fileName = modelHash;

	printf("\nBeginning to extract model...\n");
	//std::string reference = getReferenceFromHash("0174", modelHash);
	Dynamic dyn(modelHash, hash64Table, pkgsPath, bTextures);
	dyn.get();
	printf("\n\nFile extraction readied...\n");
	outputPath += "/" + fileName + "/";
	dyn.pack(outputPath);
	dyn.save(outputPath, fileName);
	std::cout << "\nFile extraction complete! Saved to" << outputPath << "/" << fileName << ".fbx\n";
	return 0;
}




/*
TODO
+ tex.txt
+ unk textures
+ texplates https://stackoverflow.com/questions/33239669/opencv-how-to-merge-two-images/45595773
+ jud VC slots
+ TGA etc export option
+ if texture already exists dont overwrite, just skip (efficiency for multiple materials same textures)
+ dont export dyemap if all black tex
- api extraction
+ batch extraction (separate folders for non-batch, joined for batch. forced no texture option)
+ texture flag
v1
*/

void doBatch(std::string pkgsPath, std::string outputPath, std::string batchPkg, std::unordered_map<uint64_t, uint32_t> hash64Table)
{
	// We need to get an array of all the valid dyn1 hashes
	Package pkg(batchPkg, pkgsPath);
	std::vector<std::string> hashes = pkg.getAllFilesGivenRef("d89a8080");
	outputPath += "/" + batchPkg + "/";
	std::cout << "\nNumber of files to batch extract: " << hashes.size() << "\n";
	for (auto& hash : hashes)
	{
		Dynamic dyn(hash, hash64Table, pkgsPath, false);
		bool status = dyn.get();
		if (status)
		{
			dyn.pack(outputPath);
			dyn.save(outputPath, hash);
			std::cout << "\nFile extraction complete! Saved to " << outputPath << "/" << hash << ".fbx\n";
		}
		else
			printf("\nEmpty file, skipping...");
	}
}
