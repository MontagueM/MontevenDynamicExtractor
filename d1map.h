#pragma once
#include "helpers.h"
#include "texture.h"
#include "vertex.h"
#include "index.h"

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
	//std::unordered_map<std::string, Materials;

	D1Map(std::string x, std::string pkgsPath) : File(x, pkgsPath)
	{
	}
	void Get();
	void ParseBakedRegionsTable();
	void GetDataTable();
};