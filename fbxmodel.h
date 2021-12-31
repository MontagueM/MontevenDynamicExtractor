#pragma once
#pragma comment(lib, "libfbxsdk.lib")
#include <fbxsdk.h>
#include "helpers.h"
#include "skeleton.h"
#include "texture.h"

class FbxModel
{
private:

	void addWeights(FbxMesh* mesh, DynamicSubmesh* submesh, std::vector <Node*> bones);

public:
	FbxMesh* createMesh(Submesh* submesh, bool bAddSkeleton);
	FbxMesh* createMesh(class Static* mesh);

	void addNorm(FbxMesh* mesh, Submesh* smesh, FbxLayer* layer);
	void addNorm(FbxMesh* mesh, class Static* smesh, FbxLayer* layer);
	template <class MeshType>
	void addUV(FbxMesh* mesh, MeshType* smesh, FbxLayer* layer)
	{
		FbxLayerElementUV* uvLayerElement = FbxLayerElementUV::Create(mesh, "uv");
		uvLayerElement->SetMappingMode(FbxLayerElement::eByControlPoint);
		uvLayerElement->SetReferenceMode(FbxLayerElement::eDirect);
		for (auto& vert : smesh->vertUV)
		{
			uvLayerElement->GetDirectArray().Add(FbxVector2(vert[0], vert[1]));
		}
		layer->SetUVs(uvLayerElement, FbxLayerElement::eTextureDiffuse);
	}
	template <class MeshType>
	void addVC(FbxMesh* mesh, MeshType* smesh, FbxLayer* layer)
	{
		FbxLayerElementVertexColor* vcLayerElement = FbxLayerElementVertexColor::Create(mesh, "vc");
		vcLayerElement->SetMappingMode(FbxLayerElement::eByControlPoint);
		vcLayerElement->SetReferenceMode(FbxLayerElement::eDirect);
		for (auto& vert : smesh->vertCol)
		{
			vcLayerElement->GetDirectArray().Add(FbxColor(vert[0], vert[1], vert[2], vert[3]));
		}
		layer->SetVertexColors(vcLayerElement);
	}
	void addVCSlots(FbxMesh* mesh, DynamicSubmesh* submesh, FbxLayer* layer);
	FbxNode* addSubmeshToFbx(DynamicSubmesh* submesh, std::vector<Node*> bones, std::string fullSavePath, bool bTextures);
	void save(std::string savePath, bool ascii);

	FbxManager* manager = FbxManager::Create();
	FbxScene* scene = FbxScene::Create(manager, "");
};