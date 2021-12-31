#include "d1map.h"

void BakedRegion::ParseMaterialTable()
{
	int MatCount;
	int TableOffset;
	memcpy((char*)&MatCount, data + 8, 4);
	memcpy((char*)&TableOffset, data + 0x10, 4);
	TableOffset += 0x20;
	for (int i = TableOffset; i < TableOffset + MatCount * 8; i += 8)
	{
		uint32_t val;
		memcpy((char*)&val, data + i + 4, 4);
		std::string MaterialHash = uint32ToHexStr(val);
		if (MaterialHash != "ffffffff" && getReferenceFromHash(MaterialHash, packagesPath) == "d71a8080")
			Materials.push_back(new Material(MaterialHash, packagesPath));
	}
}

void BakedRegion::ParseStaticsTable()
{
	int StaticCount;
	int TableOffset;
	memcpy((char*)&StaticCount, data + 0x18, 4);
	memcpy((char*)&TableOffset, data + 0x20, 4);
	TableOffset += 0x30;
	for (int i = TableOffset; i < TableOffset + StaticCount * 0x18; i += 0x18)
	{
		Static* NewStatic = new Static();
		uint32_t val;
		memcpy((char*)&val, data + i + 0x0, 4);
		std::string PosHash = uint32ToHexStr(val);
		NewStatic->vertPosFile = new VertexBufferHeader(PosHash, packagesPath, VertPrimary);
		memcpy((char*)&val, data + i + 0x4, 4);
		std::string UVHash = uint32ToHexStr(val);
		if (UVHash != "ffffffff")
			NewStatic->vertUVFile = new VertexBufferHeader(UVHash, packagesPath, VertSecondary);
		memcpy((char*)&val, data + i + 0x8, 4);
		std::string FaceHash = uint32ToHexStr(val);
		NewStatic->facesFile = new IndexBufferHeader(FaceHash, packagesPath);
		memcpy((char*)&NewStatic->LODType, data + i + 0xE, 1);
		memcpy((char*)&NewStatic->IndexStart, data + i + 0x10, 4);
		memcpy((char*)&NewStatic->IndexCount, data + i + 0x14, 4);
		NewStatic->Name = PosHash + "_" + std::to_string(NewStatic->LODType);
		NewStatic->ParentBakedRegion = this;
		Statics.push_back(NewStatic);
	}
}

void BakedRegion::GetStaticData()
{
	for (auto& s : Statics)
	{
		if (s->LODType == 1 || s->LODType == 2 || s->LODType == 0)
		{
			s->ParseVertsAndFaces();
		}
	}
}

void BakedRegion::ParseInfoTable()
{
	int InfoCount;
	int TableOffset;
	memcpy((char*)&InfoCount, data + 0x28, 4);
	memcpy((char*)&TableOffset, data + 0x30, 4);
	TableOffset += 0x40;
	for (int i = TableOffset; i < TableOffset + InfoCount * 0x18; i += 0x18)
	{
		uint16_t StaticID;
		memcpy((char*)&StaticID, data + i + 0x8, 2);
		Static* NewStatic = Statics[StaticID];
		NewStatic->ID = StaticID;
		memcpy((char*)&NewStatic->CopyCount, data + i, 1);
		memcpy((char*)&NewStatic->MaterialIndex, data + i + 0x4, 4);
		if (Materials.size() > NewStatic->MaterialIndex)
		{
			NewStatic->material = Materials[NewStatic->MaterialIndex];
		}
		memcpy((char*)&NewStatic->TransformIndex, data + i + 0xA, 4);
	}
}

void BakedRegion::Get()
{
	getData();
	ParseMaterialTable();
	ParseStaticsTable();
	ParseInfoTable();
	GetStaticData();
}

void Static::ParseVertsAndFaces()
{
	vertPosFile->vertexBuffer->getVerts(this);
	if (vertUVFile)
		vertUVFile->vertexBuffer->getVerts(this);
	facesFile->indexBuffer->getFaces(this, Triangles);
}

void D1Map::Get()
{
	getData();
	ParseBakedRegionsTable();
	GetDataTable();
	//CreateMap();
}

void D1Map::ParseBakedRegionsTable()
{
	// Five table definitions, annoying but whatever
	std::vector<int> TableCounts;
	std::vector<int> TableOffsets;
	int val;
	memcpy((char*)&val, data + 0x38, 4);
	TableCounts.push_back(val);
	memcpy((char*)&val, data + 0x40, 4);
	TableOffsets.push_back(val + 0x50);
	memcpy((char*)&val, data + 0x50, 4);
	TableCounts.push_back(val);
	memcpy((char*)&val, data + 0x58, 4);
	TableOffsets.push_back(val + 0x68);
	memcpy((char*)&val, data + 0x68, 4);
	TableCounts.push_back(val);
	memcpy((char*)&val, data + 0x70, 4);
	TableOffsets.push_back(val + 0x80);
	memcpy((char*)&val, data + 0x80, 4);
	TableCounts.push_back(val);
	memcpy((char*)&val, data + 0x88, 4);
	TableOffsets.push_back(val + 0x98);
	memcpy((char*)&val, data + 0x98, 4);
	TableCounts.push_back(val);
	memcpy((char*)&val, data + 0xA0, 4);
	TableOffsets.push_back(val + 0xB0);

	for (int i = 0; i < 5; i++)
	{
		int TableCount = TableCounts[i];
		int TableOffset = TableOffsets[i];
		for (int j = TableOffset; j < TableOffset + TableCount * 0x4; j += 0x4)
		{
			memcpy((char*)&val, data + j, 4);
			BakedRegion* BR = new BakedRegion(uint32ToHexStr(val), packagesPath);
			BR->Get();
			BakedRegions.push_back(BR);
		}
	}
}

std::vector<float> rotationMatrixToEulerAngles(std::vector<std::vector<float>> R)
{
	std::vector<float> Rotation;
	float pi = 3.14159265358979323846;
	float sy = sqrt(R[0][0] * R[0][0] + R[1][0] * R[1][0]);
	Rotation.push_back(atan2(R[2][1], R[2][2]) * 180 / pi);
	Rotation.push_back(atan2(R[2][0], sy) * 180 / pi);
	Rotation.push_back(atan2(R[1][0], R[0][0]) * 180 / pi);

	return Rotation;
}

void D1Map::GetDataTable()
{
	int Count;
	memcpy((char*)&Count, data + 0x20, 4);
	uint32_t val;
	memcpy((char*)&val, data + 0x24, 4);
	std::string Hash = uint32ToHexStr(val);
	File* DataFile = new File(Hash, packagesPath);
	DataFile->getData();
	for (int i = 0; i < Count * 0x40; i += 0x40)
	{
		// SRT
		std::vector<std::vector<float>> SRT;
		float_t fval;
		for (int j = 0; j < 4; j++)
		{
			std::vector<float> V;
			for (int k = 0; k < 16; k += 4)
			{
				memcpy((char*)&fval, DataFile->data + i + j*0x10 + k, 4);
				V.push_back(fval);
			}
			SRT.push_back(V);
		}
		
		// Translation
		std::vector<float> Translation;
		for (int j = 0; j < 3; j++)
		{
			Translation.push_back(SRT[j][3]);
		}
		Translations.push_back(Translation);

		// Scale
		std::vector<float> Scale;
		for (int j = 0; j < 3; j++)
		{
			float ScaleN = 0;
			for (int k = 0; k < 3; k++)
			{
				ScaleN += SRT[j][k] * SRT[j][k];
			}
			Scale.push_back(sqrt(ScaleN));
		}
		Scales.push_back(Scale);


		// Rotation
		std::vector<std::vector<float>> RotationMatrix;
		for (int j = 0; j < 3; j++)
		{
			std::vector<float> V;
			for (int k = 0; k < 3; k++)
			{
				V.push_back(SRT[j][k]/Scale[k]);
			}
			RotationMatrix.push_back(V);
		}
		// Rotation matrix to euler angle in degrees
		Rotations.push_back(rotationMatrixToEulerAngles(RotationMatrix));

		// UV transforms
		std::vector<float> UVTransform;
		for (int j = 0; j < 12; j += 4)
		{
			memcpy((char*)&fval, DataFile->data + i + j + 0x30, 4);
			UVTransform.push_back(fval);
		}
		UVTransforms.push_back(UVTransform);
	}
}
