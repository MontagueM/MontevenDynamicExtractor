#pragma once
#include "helpers.h"
#include <fstream>
#include <DirectXTex.h>

class Texture : public Header
{
private:
	std::string largeHash;
	std::string fullSavePath;

	void getHeader(std::string x);
	//void writeTexture(std::string fullSavePath);
	//void writeFile(DDSHeader dds, DXT10Header dxt, std::string fullSavePath);
public:
	Texture(std::string x, std::string pkgsPath) : Header(x, pkgsPath) 
	{
		getData();
		getHeader(x);
	}

	File* dataFile = nullptr;
	int textureFormat;
	uint16_t width;
	uint16_t height;
	uint16_t arraySize;
	DXGI_FORMAT dxgiFormat;
	DirectX::ScratchImage DSImage;
	
	void get();
	bool save(std::string fullSavePath, std::string saveFormat);
	//void tex2DDS(std::string fullSavePath);
	//void tex2Other(std::string fullSavePath, std::string saveFormat);
};

class Material : public File
{
private:
public:
	std::vector<std::string> cbuffers;
	std::unordered_map<uint8_t, Texture*> textures;
	Material(std::string x, std::string pkgsPath) : File(x, pkgsPath) {};

	void parseMaterial(std::unordered_map<uint64_t, uint32_t> hash64Table);
	void exportTextures(std::string fullSavePath, std::string saveFormat);
	void parseCBuffers();
	void writeCBuffers(std::string fullSavePath);
};

std::string getCBufferFromOffset(unsigned char* data, int offset, int count, uint32_t cbType, std::string name);