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
