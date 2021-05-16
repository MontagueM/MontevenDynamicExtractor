#pragma once
#pragma comment(lib, "libfbxsdk.lib")
#include <fbxsdk.h>
#include "helpers.h"
#include "skeleton.h"
#include "texture.h"

class FbxModel
{
private:
	FbxMesh* createMesh(Submesh* submesh, bool bAddSkeleton);
	void addNorm(FbxMesh* mesh, Submesh* submesh, FbxLayer* layer);
	void addUV(FbxMesh* mesh, Submesh* submesh, FbxLayer* layer);
	void addVC(FbxMesh* mesh, Submesh* submesh, FbxLayer* layer);
	void addVCSlots(FbxMesh* mesh, DynamicSubmesh* submesh, FbxLayer* layer);
	void addWeights(FbxMesh* mesh, DynamicSubmesh* submesh, std::vector <Node*> bones);

public:
	FbxNode* addSubmeshToFbx(DynamicSubmesh* submesh, std::vector<Node*> bones, std::unordered_map<uint64_t, uint32_t> hash64Table, std::string fullSavePath, bool bTextures, bool bCBuffer);
	void save(std::string savePath, bool ascii);

	FbxManager* manager = FbxManager::Create();
	FbxScene* scene = FbxScene::Create(manager, "");
};