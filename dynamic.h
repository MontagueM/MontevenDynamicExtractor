#pragma once
#include <vector>
#include <set>
#include "helpers.h"
#include "index.h"
#include "fbxmodel.h"
#include "vertex.h"

class Dynamic : public Model
{
private:
	std::vector<File> dyn2s;
	std::vector<File> dyn3s;
	FbxModel* fbxModel = nullptr;
	std::vector<DynamicMesh*> meshes;
	std::vector<FbxNode*> nodes;

	void getDyn3Files();
	void parseDyn3s();
	void getSubmeshes();

	std::vector<std::vector<float_t>> trimVertsData(std::vector<std::vector<float_t>> verts, std::set<int> dsort, bool bVertCol);
	std::vector<std::vector<uint8_t>> trimVertsData(std::vector<std::vector<uint8_t>> verts, std::set<int> dsort);

	void transformPos(DynamicMesh* mesh, unsigned char* data);
	void transformUV(DynamicMesh* mesh, unsigned char* data);
public:
	std::string packagesPath;
	Dynamic(std::string x, std::string pkgsPath) : Model(x, pkgsPath)
	{
		packagesPath = pkgsPath;
	}

	void get();
	void pack(std::string saveDirectory);
	void save(std::string saveDirectory, std::string saveName);
};

