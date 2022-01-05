#pragma once
#include "helpers.h"
#include "dxgiformat.h"
#include "vcpkg/packages/directxtex_x64-windows/include/DirectXTex.h"
#include <fstream>


struct TexHeader
{
	uint32_t TextureFormat;
	uint16_t Width;
	uint16_t Height;
	uint16_t ArraySize;
	uint32_t LargeTextureHash;
	uint8_t  flag;
};

struct DDSHeader
{
	uint32_t MagicNumber;
	uint32_t dwSize;
	uint32_t dwFlags;
	uint32_t dwHeight;
	uint32_t dwWidth;
	uint32_t dwPitchOrLinearSize;
	uint32_t dwDepth;
	uint32_t dwMipMapCount;
	std::array<uint32_t, 11> dwReserved1;
	uint32_t dwPFSize;
	uint32_t dwPFFlags;
	uint32_t dwPFFourCC;
	uint32_t dwPFRGBBitCount;
	uint32_t dwPFRBitMask;
	uint32_t dwPFGBitMask;
	uint32_t dwPFBBitMask;
	uint32_t dwPFABitMask;
	uint32_t dwCaps;
	uint32_t dwCaps2;
	uint32_t dwCaps3;
	uint32_t dwCaps4;
	uint32_t dwReserved2;
};

struct DXT10Header
{
	uint32_t dxgiFormat;
	uint32_t resourceDimension;
	uint32_t miscFlag;
	uint32_t arraySize;
	uint32_t miscFlags2;
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
	unsigned char* considerDoSwizzle(unsigned char* data, int fs, int width, int height);
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
	bool Save(std::string fullSavePath);
};

class Material : public File
{
private:
public:
	std::vector<std::string> cbuffers;
	std::unordered_map<uint8_t, Texture*> textures;
	Material(std::string x, std::string pkgsPath) : File(x, pkgsPath) {};

	void parseMaterial();
	void exportTextures(std::string fullSavePath, std::string saveFormat);
};