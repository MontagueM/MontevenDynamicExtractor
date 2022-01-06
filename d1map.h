#pragma once
#include "helpers.h"
#include "texture.h"
#include "vertex.h"
#include "index.h"
#include "fbxmodel.h"

class Static : public Mesh
{
private:
public:
	Material* material = nullptr;
	int LODType;
	int IndexStart;
	int IndexCount;
	std::string Name;
	int TransformIndex;
	int IndexStartInfo;
	int IndexCountInfo;
	int ID;
	int CopyCount;
	int MaterialIndex;
	std::string Diffuse;
	class BakedRegion* ParentBakedRegion = nullptr;

	void ParseVertsAndFaces();
};

class DynamicPoint
{
private:
public:
	std::string Name;
	std::vector<float> Translation;
};

class BakedRegion : public File
{
private:
public:
	std::vector<Static*> Statics;
	std::vector<Material*> Materials;

	void ParseMaterialTable();
	void ParseStaticsTable();
	void GetStaticData();
	void ParseInfoTable();

	void Get();

	BakedRegion(std::string x, std::string pkgsPath) : File(x, pkgsPath)
	{
	}
};

class D1Map : public File
{
private:
public:
	std::vector<BakedRegion*> BakedRegions;
	std::vector<std::vector<float>> Translations;
	std::vector<std::vector<float>> Rotations;
	std::vector<std::vector<float>> UVTransforms;
	std::vector<std::vector<float>> Scales;
	eTextureFormat TextureFormat = eTextureFormat::None;
	std::unordered_map<std::string, FbxSurfacePhong*> FbxMaterials;
	class FbxModel* fbxModel = nullptr;

	D1Map(std::string x, std::string pkgsPath, eTextureFormat InTextureFormat) : File(x, pkgsPath)
	{
		TextureFormat = InTextureFormat;
	}
	bool Get();
	void ParseBakedRegionsTable();
	void GetDataTable();
	void CreateMap(std::string Path);
	void ExportTextures(Static* Sta, std::string Path);
	void TransformUV(Static* Sta, std::vector<float> UVTransform);
	FbxMesh* AddToMap(int CopyIndex, Static* Sta, FbxMesh* mesh, std::string Path);
	void applyMaterial(FbxModel* fbxModel, Static* Sta, FbxNode* node);
	void Extract(std::string Path, std::string ExportName);
};

class D1DynMap : public File
{
private:
public:
	std::vector<DynamicPoint*> Dynamics;
	void ParseTable();
	bool Get();
	bool Extract(std::string Path, std::string ExportName);
	D1DynMap(std::string x, std::string pkgsPath) : File(x, pkgsPath)
	{
	}
};