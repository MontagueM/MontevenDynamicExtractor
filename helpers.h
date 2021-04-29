#pragma once
#include <string>
#include <unordered_map>
#include "../DestinyUnpackerCPP/package.h"

enum PrimitiveType
{
	Triangles = 3,
	TriangleStrip = 5
};

enum BufferType
{
	VertPrimary,
	VertSecondary,
	OldWeights,
	Unk,
	Index,
	VertColour,
	SPSBWeights
};

class File
{
private:

public:
	File(std::string x);

	std::string hash = "";
	unsigned char* data = nullptr;
	std::string pkgID = "";

	int getData();
};

class Header : public File
{
private:

public:
	Header(std::string x) : File(x) {}
};

class TextureHeader : public Header
{
private:

public:
};

class Material : public File
{
private:

public:
	Material(std::string x) : File(x) {};
};

class Model : public File
{
private:

public:
	Model(std::string x) : File(x) {};
};

class Submesh
{
private:
public:
	std::vector<std::vector<float>> vertPos;
	std::vector<std::vector<float>> vertNorm;
	std::vector<std::vector<float>> vertUV;
	std::vector<std::vector<float>> vertCol;
	std::vector<std::vector<uint32_t>> faces;
	int lodLevel;
	std::string name;
	Material* material;
	int type;
	int indexCount;
	int indexOffset;
	PrimitiveType primType;
};

class DynamicSubmesh : public Submesh
{
private:
public:
	std::vector<std::vector<float>> weights;
	int stride;
	TextureHeader* diffuse = nullptr;
	int gearDyeChangeColourIndex;
	int alphaClip;
	int lodGroup;
};

// Forward declarations
class IndexBufferHeader;
class VertexBufferHeader;

class Mesh
{
private:
public:
	//Mesh() {};
	IndexBufferHeader* facesFile = nullptr;
	VertexBufferHeader* vertPosFile = nullptr;
	std::vector<std::vector<float>> vertPos;
	std::vector<std::vector<float>> vertNorm;
	std::vector<std::vector<float>> vertUV;
	std::vector<std::vector<float>> vertCol;
	std::vector<std::vector<uint32_t>> faces;
	std::unordered_map<int, int> faceMap;
	std::vector<Submesh*> submeshes;
};

class DynamicMesh : public Mesh
{
private:
public:
	//DynamicMesh() : Mesh() {};
	std::vector<std::vector<float>> vertPosW;
	std::vector<std::vector<float>> vertNormW;
	std::vector<std::vector<float>> weights;
	std::vector<DynamicSubmesh*> submeshes;
};

std::string getReferenceFromHash(std::string hash);
std::string getPkgID(std::string hash);