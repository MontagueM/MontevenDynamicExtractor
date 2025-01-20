#pragma once
#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <fstream>


bool getLatestPatchIDs_Str(std::vector<std::string>& latestPackages, std::string packagesPath);
bool getLatestPatchIDs_Bin(std::vector<std::string>& latestPackages, std::string packagesPath);
std::vector<std::string> getLatestPatchIDs(std::string packagesPath);

std::string packagesPath;


