#pragma once
#include <vector>
#include <set>
#include "helpers.h"
#include "index.h"
#include "fbxmodel.h"
#include "vertex.h"
#include "texture.h"
#include "skeleton.h"
#include "texplate.h"

class Dynamic : public Model
{
private:
	bool bTextures;
	std::vector<File*> dyn2s;
	std::vector<File*> dyn3s;
	FbxModel* fbxModel = nullptr;
	std::vector<DynamicMesh*> meshes;
	std::vector<FbxNode*> nodes;
	std::vector<Node*> bones;
	std::string skeletonHash = "";
	bool bSkeletonDiostOnly = false;
	std::unordered_map<uint64_t, uint32_t> h64Table;
	std::vector<Material*> externalMaterials;
	std::vector<TexturePlateSet*> texplateSets;
	int skeletonOverride = -1;

	void getDyn3Files();
	void parseDyn3s();
	void getSubmeshes();
	void getSkeleton();
	void getTexturePlates();

	std::vector<std::vector<float_t>> trimVertsData(std::vector<std::vector<float_t>> verts, std::set<int> dsort, bool bVertCol);
	std::vector<std::vector<uint8_t>> trimVertsData(std::vector<std::vector<uint8_t>> verts, std::set<int> dsort);

	void transformPos(DynamicMesh* mesh, unsigned char* data);
	void transformUV(DynamicMesh* mesh, unsigned char* data);
	void addVertColSlots(DynamicMesh* mesh, DynamicSubmesh* submesh);

	void considerSkeletonOverride();
public:
	std::string packagesPath;
	Dynamic(std::string x, std::unordered_map<uint64_t, uint32_t> hash64Table, std::string pkgsPath, bool btex, int so) : Model(x, pkgsPath)
	{
		packagesPath = pkgsPath;
		h64Table = hash64Table;
		bTextures = btex;
		skeletonOverride = so;
	}

	bool get();
	void pack(std::string saveDirectory, bool bCBuffer);
	void save(std::string saveDirectory, std::string saveName);
};

