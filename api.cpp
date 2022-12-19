#include "api.h"

std::unordered_map<uint32_t, std::string> channelNames =
{
	{662199250, "ArmorPlate"},
	{1367384683, "ArmorSuit"},
	{218592586, "ArmorCloth"},
	{1667433279, "Weapon1"},
	{1667433278, "Weapon2"},
	{1667433277, "Weapon3"},
	{3073305669, "ShipUpper"},
	{3073305668, "ShipDecals"},
	{3073305671, "ShipLower"},
	{1971582085, "SparrowUpper"},
	{1971582084, "SparrowEngine"},
	{1971582087, "SparrowLower"},
	{373026848, "GhostMain"},
	{373026849, "GhostHighlights"},
	{373026850, "GhostDecals"},
};

std::vector<std::string> dataNames =
{
	"\"detail_diffuse_transform\"",
	"\"detail_normal_transform\"",
	"\"spec_aa_transform\"",
	"\"primary_albedo_tint\"",
	"\"primary_emissive_tint_color_and_intensity_bias\"",
	"\"primary_material_params\"",
	"\"primary_material_advanced_params\"",
	"\"primary_roughness_remap\"",
	"\"primary_worn_albedo_tint\"",
	"\"primary_wear_remap\"",
	"\"primary_worn_roughness_remap\"",
	"\"primary_worn_material_parameters\"",
	"\"secondary_albedo_tint\"",
	"\"secondary_emissive_tint_color_and_intensity_bias\"",
	"\"secondary_material_params\"",
	"\"secondary_material_advanced_params\"",
	"\"secondary_roughness_remap\"",
	"\"secondary_worn_albedo_tint\"",
	"\"secondary_wear_remap\"",
	"\"secondary_worn_roughness_remap\"",
	"\"secondary_worn_material_parameters\"",
};

std::vector<std::string> getAPIModelHashes(uint32_t apiHash, std::string packagesPath, std::unordered_map<uint64_t, uint32_t> hash64Table, bool& bSingle)
{
	std::vector<std::string> modelHashes;
	File* modelTable = new File("0A4CCE80", packagesPath);
	modelTable->getData();
	// Finding val
	uint32_t tableOffset = 0x40;
	uint32_t tableCount;
	memcpy((char*)&tableCount, modelTable->data + 8, 4);
	uint32_t val;

	apiHash = getArtArrangementHash(apiHash, packagesPath);

	for (int i = tableOffset; i < tableOffset + tableCount * 0x20; i += 0x20)
	{
		memcpy((char*)&val, modelTable->data + i, 4);
		if (val == apiHash)
		{
			memcpy((char*)&val, modelTable->data + i + 0x18, 4);
			if (val != 0) modelHashes = getAPIMultiHashes(val + i + 0x18, modelTable, packagesPath, hash64Table);
			else
			{
				uint32_t fHash;
				memcpy((char*)&val, modelTable->data + i + 0x8, 4);
				memcpy((char*)&fHash, modelTable->data + i + 0xC, 4);
				modelHashes = getAPISingleHashes(val, fHash, packagesPath, hash64Table);
				bSingle = true;
			}
			break;
		}
	}
	delete modelTable;
	return modelHashes;
}

uint32_t getArtArrangementHash(uint32_t apiHash, std::string packagesPath)
{
	File* dataTable = new File("6260AE80", packagesPath);
	dataTable->getData();
	File* arrangementTable = new File("0D60AE80", packagesPath);
	arrangementTable->getData();

	uint32_t tableOffset = 0x30;
	uint32_t tableCount;
	uint32_t val;
	uint32_t val2;
	memcpy((char*)&tableCount, dataTable->data + 8, 4);
	for (int i = tableOffset; i < tableOffset + tableCount * 0x20; i += 0x20)
	{
		memcpy((char*)&val, dataTable->data + i, 4);
		if (val == apiHash)
		{
			memcpy((char*)&val, dataTable->data + i + 0x10, 4);
			File dataFile = File(uint32ToHexStr(val), packagesPath);
			dataFile.getData();
			memcpy((char*)&val, dataFile.data + 0x90, 4);
			val += 0x90 + 8;
			memcpy((char*)&val2, dataFile.data + val, 4);
			val += val2 + 0x12;
			memcpy((char*)&val2, dataFile.data + val, 2);
			memcpy((char*)&val2, arrangementTable->data + val2 * 4 + 48, 4);
			delete dataTable;
			delete arrangementTable;
			return val2;
		}
	}

	// No art-arrangement hash, let's just try anyway.
	delete arrangementTable;
	return apiHash;
}


bool getAPIShader(uint32_t apiHash, std::string outputPath, std::string packagesPath, std::unordered_map<uint64_t, uint32_t> hash64Table)
{
	File* dataTable = new File("6260AE80", packagesPath);
	dataTable->getData();

	uint32_t tableOffset = 0x30;
	uint32_t tableCount;
	uint32_t val;
	uint32_t val2;
	std::unordered_map<uint16_t, uint16_t> defaultChannelDyeMap;
	std::unordered_map<uint16_t, uint16_t> customChannelDyeMap;
	memcpy((char*)&tableCount, dataTable->data + 8, 4);
	for (int i = tableOffset; i < tableOffset + tableCount * 0x20; i += 0x20)
	{
		memcpy((char*)&val, dataTable->data + i, 4);
		if (val == apiHash)
		{
			memcpy((char*)&val, dataTable->data + i + 0x10, 4);
			File dataFile = File(uint32ToHexStr(val), packagesPath);
			dataFile.getData();
			memcpy((char*)&val, dataFile.data + 0x90, 4);
			val += 0x90;
			memcpy((char*)&val2, dataFile.data + val - 4, 4);
			if (val2 != 0x80807377)
			{
				printf("Given shader is not valid!\n");
				return false;
			}
			uint32_t defaultDyeTableCount;
			uint32_t defaultDyeTableOffset;
			uint32_t customDyeTableCount;
			uint32_t customDyeTableOffset;
			memcpy((char*)&defaultDyeTableCount, dataFile.data + val + 0x28, 4);
			memcpy((char*)&defaultDyeTableOffset, dataFile.data + val + 0x30, 4);
			defaultDyeTableOffset += 0x10 + val + 0x30;
			memcpy((char*)&customDyeTableCount, dataFile.data + val + 0x38, 4);
			memcpy((char*)&customDyeTableOffset, dataFile.data + val + 0x40, 4);
			customDyeTableOffset += 0x10 + val + 0x40;
			uint16_t channelIndex;
			uint16_t dyeIndex;
			for (int j = defaultDyeTableOffset; j < defaultDyeTableOffset + defaultDyeTableCount * 4; j += 4)
			{
				memcpy((char*)&channelIndex, dataFile.data + j, 2);
				memcpy((char*)&dyeIndex, dataFile.data + j + 2, 2);
				defaultChannelDyeMap[channelIndex] = dyeIndex;
			}
			for (int j = customDyeTableOffset; j < customDyeTableOffset + customDyeTableCount * 4; j += 4)
			{
				memcpy((char*)&channelIndex, dataFile.data + j, 2);
				memcpy((char*)&dyeIndex, dataFile.data + j + 2, 2);
				customChannelDyeMap[channelIndex] = dyeIndex;
			}
		}
	}
	if (defaultChannelDyeMap.size() == 0) return false;

	File* channelTable = new File("C92FCF80", packagesPath);
	channelTable->getData();
	File* dyeManifestTable = new File("664ECE80", packagesPath);
	dyeManifestTable->getData();
	File* dyeFileTable = new File("BDB2C180", packagesPath);
	dyeFileTable->getData();
	uint32_t channelHash;
	uint32_t dyeManifestHash;
	std::string dyeFileHash;
	File* finalDyeFile = nullptr;
	std::string channelName;
	std::string channelNameHash;
	// For each pair, find the channel hash to pair it with a name + find the dye file

	std::unordered_map<std::string, std::unordered_map<std::string, std::vector<float>>> defaultDyes;
	std::unordered_map<std::string, std::unordered_map<std::string, std::vector<float>>> customDyes;

	std::unordered_map<std::string, std::unordered_map<std::string, std::string>> defaultTextures;
	std::unordered_map<std::string, std::unordered_map<std::string, std::string>> customTextures;

	for (int q = 0; q < 2; q++)
	{
		auto map = customChannelDyeMap;
		if (q == 0) auto map = defaultChannelDyeMap;

		for (auto& it : map)
		{
			finalDyeFile = nullptr;

			// Get channel name
			memcpy((char*)&channelHash, channelTable->data + 0x30 + 4 * it.first, 4);
			channelName = channelNames[channelHash];
			channelNameHash = std::to_string(channelHash);

			// Get dye file
			memcpy((char*)&dyeManifestHash, dyeManifestTable->data + 0x30 + 8 * it.second + 4, 4);
			tableOffset = 0x40;
			memcpy((char*)&tableCount, dyeFileTable->data + 8, 4);
			for (int i = tableOffset; i < tableOffset + tableCount * 0x18; i += 0x18)
			{
				memcpy((char*)&val, dyeFileTable->data + i, 4);
				if (val == dyeManifestHash)
				{
					memcpy((char*)&val, dyeFileTable->data + i + 0xC, 4);
					if (val == 1)
					{
						uint32_t dyeFileID;
						memcpy((char*)&dyeFileID, dyeFileTable->data + i + 0x8, 4);
						dyeFileHash = uint32ToHexStr(dyeFileID);
					}
					else
					{
						// H64
						uint64_t h64;
						memcpy((char*)&h64, dyeFileTable->data + i + 0x10, 8);
						dyeFileHash = getHash64(h64, hash64Table);
					}

					File file = File(dyeFileHash, packagesPath);
					size_t fileSize = file.getData();
					if (fileSize != 0x18) break;
					uint32_t dyeFileID;
					memcpy((char*)&dyeFileID, file.data + 0x8, 4);
					dyeFileHash = uint32ToHexStr(dyeFileID);
					file = File(dyeFileHash, packagesPath);
					fileSize = file.getData();
					if (fileSize < 0x10) break;
					memcpy((char*)&dyeFileID, file.data + 0xC, 4);
					dyeFileHash = uint32ToHexStr(dyeFileID);
					finalDyeFile = new File(dyeFileHash, packagesPath);
					finalDyeFile->getData();
					break;
				}
			}
			// Get dye textures
			memcpy((char*)&tableCount, finalDyeFile->data + 0x48, 4);
			memcpy((char*)&tableOffset, finalDyeFile->data + 0x50, 4);
			tableOffset += 0x60;
			std::string texHash;
			std::string diffuseName;
			std::string normalName;
			for (int i = tableOffset; i < tableOffset + tableCount * 0x18; i += 0x18)
			{
				// H64
				uint64_t h64;
				memcpy((char*)&h64, finalDyeFile->data + i + 0x10, 8);
				texHash = getHash64(h64, hash64Table);
				uint32_t texID;
				memcpy((char*)&texID, finalDyeFile->data + i, 4);

				// Save texture
				Texture tex = Texture(texHash, packagesPath);
				std::string addString = "dif";
				if (texID % 2 != 0)
				{
					addString = "norm";
					normalName = channelName + "_" + std::to_string(texID) + "_" + texHash + "_" + addString;
				}
				else diffuseName = channelName + "_" + std::to_string(texID) + "_" + texHash + "_" + addString;
				tex.save(outputPath + "/" + channelName + "_" + std::to_string(texID) + "_" + texHash + "_" + addString, "png");
			}
			// Get dye data
			if (finalDyeFile == nullptr) continue;
			std::unordered_map<std::string, std::vector<float>> dyeData;
			std::unordered_map<std::string, std::string> texData;
			memcpy((char*)&tableCount, finalDyeFile->data + 0x90, 4);
			memcpy((char*)&tableOffset, finalDyeFile->data + 0x98, 4);
			tableOffset += 0xA8;
			float_t fval;
			int c = 0;
			for (int i = tableOffset; i < tableOffset + tableCount * 0x10; i += 0x10)
			{
				std::vector<float> data;
				for (int j = 0; j < 4; j++)
				{
					memcpy((char*)&fval, finalDyeFile->data + i + j * 4, 4);
					data.push_back(fval);
				}
				dyeData[dataNames[c]] = data;
				c++;
			}


			if (q == 0)
			{
				defaultDyes[channelNameHash] = dyeData;
				texData["Diffuse"] = diffuseName;
				texData["Normal"] = normalName;
				defaultTextures[channelNameHash] = texData;
			}
			else
			{
				customDyes[channelNameHash] = dyeData;
				texData["Diffuse"] = diffuseName;
				texData["Normal"] = normalName;
				customTextures[channelNameHash] = texData;
			}
		}
	}

	for (int q = 0; q < 2; q++)
	{
		if (q == 0) writeShader(defaultDyes, defaultTextures, false, outputPath);
		else writeShader(customDyes, customTextures, true, outputPath);
	}
	return true;
}

void writeShader(std::unordered_map<std::string, std::unordered_map<std::string, std::vector<float>>> dyes, std::unordered_map<std::string, std::unordered_map<std::string, std::string>> textures, bool bCustom, std::string outputPath)
{
	std::string stringFactoryShader = "{\n";
	if (!bCustom) stringFactoryShader += "  \"custom_dyes\": [],\n  \"locked_dyes\": [],\n  \"default_dyes\": [\n";
	else stringFactoryShader += "  \"default_dyes\": [],\n  \"locked_dyes\": [],\n  \"custom_dyes\": [\n";
	std::string propertiesString = "";
	for (auto& it : dyes)
	{
		//propertiesString += "    	" + it.first + ":\n";
		propertiesString += "    {\n";
		propertiesString += "      \"investment_hash\": " + it.first + ",\n";
		if (it.first.find("Cloth", 0) != std::string::npos) propertiesString += "      \"cloth\": true,\n";
		else propertiesString += "      \"cloth\": false,\n";
		propertiesString += "      \"material_properties\": {\n";
		std::string valuesString = "";
		for (auto& it2 : it.second)
		{
			if (it.first.find("diffuse", 0) != std::string::npos) break;
			std::string floatString = "[";
			for (auto& flt : it2.second) floatString += std::to_string(flt) + ", ";
			//stringFactoryShader += "			" + it2.first + ": " + floatString.substr(0, floatString.size()-2) + "],\n";
			valuesString += "        " + it2.first + ": " + floatString.substr(0, floatString.size() - 2) + "],\n";
		}
		propertiesString += valuesString.substr(0, valuesString.size() - 2) + "\n      },\n";
		propertiesString += "      \"textures\": {\n";
		propertiesString += "        \"diffuse\": {\n          \"name\": \"" + textures[it.first]["Diffuse"] + "\"\n        },\n";
		propertiesString += "        \"normal\": {\n          \"name\": \"" + textures[it.first]["Normal"] + "\"\n        }\n";
		propertiesString += "      }\n    },\n";
	}
	stringFactoryShader += propertiesString.substr(0, propertiesString.size() - 2) + "\n  ]\n}";
	//stringFactoryShader += "\n";

	FILE* shaderFile;
	std::string path = outputPath + "/shader.json";
	fopen_s(&shaderFile, path.c_str(), "w");
	fwrite(stringFactoryShader.c_str(), stringFactoryShader.size(), 1, shaderFile);
	fclose(shaderFile);
}


std::vector<std::string> getAPISingleHashes(uint32_t mHash, uint32_t fHash, std::string packagesPath, std::unordered_map<uint64_t, uint32_t> hash64Table)
{
	std::vector<std::string> h64Files = { "", "" };
	File pairTable = File("BEB2C180", packagesPath);
	pairTable.getData();
	uint32_t tableOffset = 0x30;
	uint32_t tableCount;
	memcpy((char*)&tableCount, pairTable.data + 8, 4);
	uint32_t val;
	for (int i = tableOffset; i < tableOffset + tableCount * 8; i += 8)
	{
		memcpy((char*)&val, pairTable.data + i, 4);
		if (val == mHash)
		{
			memcpy((char*)&val, pairTable.data + i + 4, 4);
			h64Files[0] = uint32ToHexStr(val);
		}
		else if (val == fHash)
		{
			memcpy((char*)&val, pairTable.data + i + 4, 4);
			h64Files[1] = uint32ToHexStr(val);
		}
		if (h64Files[0] != "" && h64Files[1] != "") break;
	}

	return getHashesFromH64s(h64Files, packagesPath, hash64Table);;
}

std::vector<std::string> getAPIMultiHashes(uint32_t tableOffset, File* modelTable, std::string packagesPath, std::unordered_map<uint64_t, uint32_t> hash64Table)
{
	uint32_t val1;
	uint32_t val2;
	std::vector<uint32_t> offsets;
	std::vector<std::string> h64Files;
	std::vector<std::uint32_t> pairFiles;
	uint32_t tableCount;
	memcpy((char*)&tableCount, modelTable->data + tableOffset, 4);
	tableOffset += 0x10;
	for (int i = tableOffset; i < tableOffset + tableCount * 8; i += 8)
	{
		memcpy((char*)&val1, modelTable->data + i, 4);
		val1 += i + 0x10;
		memcpy((char*)&val2, modelTable->data + val1, 4);
		val2 += val1;
		uint32_t tableCount2;
		memcpy((char*)&tableCount2, modelTable->data + val2, 4);
		val2 += 0x10;
		uint32_t val3;
		for (int j = val2; j < val2 + tableCount2 * 4; j += 4)
		{
			memcpy((char*)&val3, modelTable->data + j, 4);
			pairFiles.push_back(val3);
		}
	}

	File pairTable = File("BEB2C180", packagesPath);
	pairTable.getData();
	uint32_t table2Offset = 0x30;
	uint32_t table2Count;
	memcpy((char*)&table2Count, pairTable.data + 8, 4);
	uint32_t val;
	for (int i = table2Offset; i < table2Offset + table2Count * 8; i += 8)
	{
		memcpy((char*)&val, pairTable.data + i, 4);
		for (auto& hash : pairFiles)
		{
			if (val == hash)
			{
				memcpy((char*)&val, pairTable.data + i + 4, 4);
				h64Files.push_back(uint32ToHexStr(val));
			}
		}
	}

	return getHashesFromH64s(h64Files, packagesPath, hash64Table);
}

std::vector<std::string> getHashesFromH64s(std::vector<std::string> h64Files, std::string packagesPath, std::unordered_map<uint64_t, uint32_t> hash64Table)
{
	std::vector<std::string> modelHashes;
	uint64_t h64;
	for (auto& f : h64Files)
	{
		if (f == "") continue;
		File file = File(f, packagesPath);
		file.getData();
		memcpy((char*)&h64, file.data + 0x10, 8);
		std::string hash = getHash64(h64, hash64Table);
		modelHashes.push_back(hash);
	}
	return modelHashes;
}
