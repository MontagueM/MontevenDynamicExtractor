#pragma once
#pragma comment(lib, "libfbxsdk.lib")
#include <fbxsdk.h>
#include "helpers.h"

class FbxModel
{
private:
	FbxManager* manager = FbxManager::Create();

public:
	FbxNode* addSubmeshToFbx(Submesh* submesh, std::string fullSavePath);
	void save(std::string savePath, bool ascii);
	FbxMesh* createMesh(Submesh* submesh);

	FbxScene* scene = FbxScene::Create(manager, "");
};