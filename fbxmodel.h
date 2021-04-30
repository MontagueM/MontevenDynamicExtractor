#pragma once
#pragma comment(lib, "libfbxsdk.lib")
#include <fbxsdk.h>
#include "helpers.h"

class FbxModel
{
private:
	FbxManager* manager = FbxManager::Create();
	FbxMesh* createMesh(Submesh* submesh);
	void addNorm(FbxMesh* mesh, Submesh* submesh, FbxLayer* layer);
	void addUV(FbxMesh* mesh, Submesh* submesh, FbxLayer* layer);
	void addVC(FbxMesh* mesh, Submesh* submesh, FbxLayer* layer);

public:
	FbxNode* addSubmeshToFbx(Submesh* submesh, std::string fullSavePath);
	void save(std::string savePath, bool ascii);

	FbxScene* scene = FbxScene::Create(manager, "");
};