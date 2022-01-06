#pragma once
#include "helpers.h"
#include "vcpkg/packages/directxtex_x64-windows/include/DirectXTex.h"
#include <fstream>

enum eTextureFormat
{
	None = 1,
	DDS = 2,
	TGA = 3,
	PNG = 4,
};

class Texture : public Header
{
private:

	int bytesPerPixel;
	int pixelBlockSize;

	uint16_t arraySize;
	uint8_t  flag;
	std::string largeHash;
	std::string fullSavePath;

	void getHeader(std::string x);
	void considerDoSwizzle(unsigned char* data, int fs, int width, int height);
public:
	File* dataFile = nullptr;
	DXGI_FORMAT dxgiFormat;
	int textureFormat;
	uint16_t width;
	uint16_t height;
	DirectX::ScratchImage DSImage;

	Texture(std::string x, std::string pkgsPath) : Header(x, pkgsPath)
	{
		getData();
		getHeader(x);
	}
	void Get();
	bool Save(std::string fullSavePath, eTextureFormat TextureFormat);
};

class Material : public File
{
private:
public:
	std::vector<std::string> cbuffers;
	std::unordered_map<uint8_t, Texture*> textures;
	Material(std::string x, std::string pkgsPath) : File(x, pkgsPath) {};

	void parseMaterial();
	void exportTextures(std::string fullSavePath, eTextureFormat TextureFormat);
};