#pragma once
#include "helpers.h"
#include "texture.h"
#include <unordered_map>

std::vector<std::string> getAPIModelHashes(uint32_t apiHash, std::string packagesPath, std::unordered_map<uint64_t, uint32_t> hash64Table, bool& bSingle);
std::vector<std::string> getAPISingleHashes(uint32_t mHash, uint32_t fHash, std::string packagesPath, std::unordered_map<uint64_t, uint32_t> hash64Table);
std::vector<std::string> getAPIMultiHashes(uint32_t tableOffset, File* modelTable, std::string packagesPath, std::unordered_map<uint64_t, uint32_t> hash64Table);
std::vector<std::string> getHashesFromH64s(std::vector<std::string> h64Files, std::string packagesPath, std::unordered_map<uint64_t, uint32_t> hash64Table);
uint32_t getArtArrangementHash(uint32_t apiHash, std::string packagesPath);
bool getAPIShader(uint32_t apiHash, std::string outputPath, std::string packagesPath, std::unordered_map<uint64_t, uint32_t> hash64Table);
void writeShader(std::unordered_map<std::string, std::unordered_map<std::string, std::vector<float>>> dyes, std::unordered_map<std::string, std::unordered_map<std::string, std::string>> textures, bool bCustom, std::string outputPath);

const std::string InventoryItemMapHash = "14FADB80"; // 97798080 / 80807997
const std::string ArtArrangementMapHash = "0FA0A780"; // F2708080 / 808070F2
const std::string EntityAssignmentMapHash = "C1A1A780"; // CE558080 / 808055CE
const std::string DyeChannelHash = "0DA0A780"; // 68768080 / 80807668
const std::string DyeManifestHash = "C2A1A780"; // C2558080 / 808055C2
const std::string DyeFileHash = "A28EA780"; // 8C978080 / 8080978C
const std::string PairTableHash = "A38EA780"; // 434F8080 / 80804F43