#include "fbxmodel.h"

FbxNode* FbxModel::addSubmeshToFbx(DynamicSubmesh* submesh, std::vector<Node*> bones, std::unordered_map<uint64_t, uint32_t> hash64Table, std::string fullSavePath, bool bTextures, bool bCBuffer)
{
	bool bAddSkeleton = submesh->weights.size() && bones.size();

	FbxMesh* mesh = createMesh(submesh, bAddSkeleton);
	if (!mesh->GetLayer(0)) mesh->CreateLayer();
	FbxLayer* layer = mesh->GetLayer(0);
	FbxNode* node = FbxNode::Create(manager, submesh->name.c_str());
	node->SetNodeAttribute(mesh);
	node->LclScaling.Set(FbxDouble3(100, 100, 100));
	if (submesh->vertNorm.size()) addNorm(mesh, submesh, layer);
	if (submesh->vertUV.size()) addUV(mesh, submesh, layer);
	if (submesh->vertCol.size()) addVC(mesh, submesh, layer);
	if (submesh->vertColSlots.size())
	{
		mesh->CreateLayer();
		FbxLayer* layerVC = mesh->GetLayer(1);
		addVCSlots(mesh, submesh, layerVC);
	}
	if (bAddSkeleton) addWeights(mesh, submesh, bones);
	if (submesh->material != nullptr && bTextures)
	{
		std::filesystem::create_directories(fullSavePath + "/textures/");

		submesh->material->parseMaterial(hash64Table);
		FILE* texFile;
		std::string path = fullSavePath + "/textures/tex.txt";
		fopen_s(&texFile, path.c_str(), "a");
		std::string toWrite = submesh->name + ":MAT[" + submesh->material->hash + "]: ";
		for (auto& tex : submesh->material->textures)
		{
			toWrite += tex.second->hash + ", ";
		}
		toWrite = toWrite.substr(0, toWrite.size() - 2);
		toWrite += "\n";
		fwrite(toWrite.c_str(), toWrite.size(), 1, texFile);
		fclose(texFile);
		submesh->material->exportTextures(fullSavePath + "/textures/", "png");
	}
	
	if (submesh->material != nullptr && bCBuffer)
	{
		submesh->material->writeCBuffers(fullSavePath);
	}

	return node;
}

void FbxModel::addNorm(FbxMesh* mesh, Submesh* submesh, FbxLayer* layer)
{
	FbxLayerElementNormal* normLayerElement = FbxLayerElementNormal::Create(mesh, "norm");
	normLayerElement->SetMappingMode(FbxLayerElement::eByControlPoint);
	normLayerElement->SetReferenceMode(FbxLayerElement::eDirect);
	for (auto& vert : submesh->vertNorm)
	{
		normLayerElement->GetDirectArray().Add(FbxVector4(-vert[0], vert[2], vert[1]));
	}
	layer->SetNormals(normLayerElement);
}

void FbxModel::addUV(FbxMesh* mesh, Submesh* submesh, FbxLayer* layer)
{
	FbxLayerElementUV* uvLayerElement = FbxLayerElementUV::Create(mesh, "uv");
	uvLayerElement->SetMappingMode(FbxLayerElement::eByControlPoint);
	uvLayerElement->SetReferenceMode(FbxLayerElement::eDirect);
	for (auto& vert : submesh->vertUV)
	{
		uvLayerElement->GetDirectArray().Add(FbxVector2(vert[0], vert[1]));
	}
	layer->SetUVs(uvLayerElement, FbxLayerElement::eTextureDiffuse);
}

void FbxModel::addVC(FbxMesh* mesh, Submesh* submesh, FbxLayer* layer)
{
	FbxLayerElementVertexColor* vcLayerElement = FbxLayerElementVertexColor::Create(mesh, "vc");
	vcLayerElement->SetMappingMode(FbxLayerElement::eByControlPoint);
	vcLayerElement->SetReferenceMode(FbxLayerElement::eDirect);
	for (auto& vert : submesh->vertCol)
	{
		vcLayerElement->GetDirectArray().Add(FbxColor(vert[0], vert[1], vert[2], vert[3]));
	}
	layer->SetVertexColors(vcLayerElement);
}

void FbxModel::addVCSlots(FbxMesh* mesh, DynamicSubmesh* submesh, FbxLayer* layer)
{
	FbxLayerElementVertexColor* vcLayerElement = FbxLayerElementVertexColor::Create(mesh, "slots");
	vcLayerElement->SetMappingMode(FbxLayerElement::eByControlPoint);
	vcLayerElement->SetReferenceMode(FbxLayerElement::eDirect);
	for (auto& vert : submesh->vertColSlots)
	{
		vcLayerElement->GetDirectArray().Add(FbxColor(vert[0], vert[1], vert[2], vert[3]));
	}
	layer->SetVertexColors(vcLayerElement);
}

void FbxModel::addWeights(FbxMesh* mesh, DynamicSubmesh* submesh, std::vector <Node*> bones)
{
	FbxSkin* skin = FbxSkin::Create(manager, submesh->name.c_str());
	std::vector< FbxCluster*> boneCluster;
	boneCluster.reserve(bones.size());

	for (auto& bone : bones)
	{
		FbxCluster* weightCluster = FbxCluster::Create(manager, "BoneWeightCluster");
		weightCluster->SetLink(bone->fbxNode);
		weightCluster->SetLinkMode(FbxCluster::eTotalOne);
		FbxAMatrix transform = bone->fbxNode->EvaluateGlobalTransform();
		weightCluster->SetTransformLinkMatrix(transform);
		boneCluster.push_back(weightCluster);
	}

	for (int i = 0; i < submesh->weights.size(); i++)
	{
		std::vector<uint8_t> indices = submesh->weightIndices[i];
		std::vector<float> weights = submesh->weights[i];
		for (int j = 0; j < indices.size(); j++)
		{
			if (boneCluster.size() < indices[j])
			{
				printf("Bone index longer than bone clusters, could not add weights!");
				exit(1);
			}
			boneCluster[indices[j]]->AddControlPointIndex(i, weights[j]);
		}
	}

	for (auto& c : boneCluster)
		skin->AddCluster(c);

	mesh->AddDeformer(skin);
}

FbxMesh* FbxModel::createMesh(Submesh* submesh, bool bAddSkeleton)
{
	FbxMesh* mesh = FbxMesh::Create(manager, submesh->name.c_str());
	for (int i = 0; i < submesh->vertPos.size(); i++)
	{
		std::vector<float_t> v = submesh->vertPos[i];
		if (bAddSkeleton)
			mesh->SetControlPointAt(FbxVector4(-v[0] * 100, v[2] * 100, v[1] * 100), i);
		else
			mesh->SetControlPointAt(FbxVector4(-v[0], v[2], v[1]), i);
	}
	for (int f = 0; f < submesh->faces.size(); f++)
	{
		if (!submesh->faces[f].size())
		{
			std::perror("why is it failing here\n");
			continue;
		}
		std::vector<uint32_t> face = submesh->faces[f];
		//std::cout << std::to_string(f) << ": " << std::to_string(face[0]) << " " << std::to_string(face[1]) << " " << std::to_string(face[2]) << "\n";
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
