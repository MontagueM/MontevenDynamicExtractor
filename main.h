#pragma once
#include <string>
#include <iostream>
#include <stdio.h> 
#include "Sarge/src/sarge.cpp"
#include "dynamic.h"
#include "../DestinyUnpackerCPP/package.h"
#include <fstream>

void doBatch(std::string pkgsPath, std::string outputPath, std::string batchPkg, std::unordered_map<uint64_t, uint32_t> hash64Table);