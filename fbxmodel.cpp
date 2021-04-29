#include "fbxmodel.h"

FbxNode* FbxModel::addSubmeshToFbx(Submesh* submesh, std::string fullSavePath)
{
	FbxMesh* mesh = createMesh(submesh);
	if (!mesh->GetLayer(0)) mesh->CreateLayer();
	FbxLayer* layer = mesh->GetLayer(0);
	FbxNode* node = FbxNode::Create(manager, submesh->name.c_str());
	node->SetNodeAttribute(mesh);
	node->LclScaling.Set(FbxDouble3(100, 100, 100));
	return node;
}

FbxMesh* FbxModel::createMesh(Submesh* submesh)
{
	FbxMesh* mesh = FbxMesh::Create(manager, submesh->name.c_str());
	for (int i = 0; i < submesh->vertPos.size(); i++)
	{
		std::vector<float_t> v = submesh->vertPos[i];
		mesh->SetControlPointAt(FbxVector4(-v[0], v[2], v[1]), i);
	}
	for (auto& face : submesh->faces)
	{
		mesh->BeginPolygon();
		mesh->AddPolygon(face[0]);
		mesh->AddPolygon(face[1]);
		mesh->AddPolygon(face[2]);
		mesh->EndPolygon();
	}
	return mesh;
}

void FbxModel::save(std::string savePath, bool ascii)
{
	if (not manager->GetIOSettings())
	{
		FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
		manager->SetIOSettings(ios);
	}

	manager->GetIOSettings()->SetBoolProp(EXP_FBX_MATERIAL, true);
	manager->GetIOSettings()->SetBoolProp(EXP_FBX_TEXTURE, true);
	manager->GetIOSettings()->SetBoolProp(EXP_FBX_EMBEDDED, true);
	manager->GetIOSettings()->SetBoolProp(EXP_FBX_SHAPE, true);
	manager->GetIOSettings()->SetBoolProp(EXP_FBX_GOBO, true);
	manager->GetIOSettings()->SetBoolProp(EXP_FBX_ANIMATION, true);
	manager->GetIOSettings()->SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);
	int intAscii = -1;
	if (ascii) intAscii = 1;

	FbxExporter* exporter = FbxExporter::Create(manager, "");
	exporter->Initialize(savePath.c_str(), intAscii, manager->GetIOSettings());
	exporter->Export(scene);
	exporter->Destroy();
}