#pragma once
#include <unordered_map>
#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs.hpp"
#include "helpers.h"
#include "texture.h"

/*
- Parse texture plate set to get each plate
- Parse each plate to get their coords, scales, and tex refs
- Extract each texture from a plate
- Open in opencv and combine using texplate data
- Save as a final image
*/

class TexplateTexture : public Texture
{
private:
public:
	uint32_t offsetX;
	uint32_t offsetY;
	uint32_t scaleX;
	uint32_t scaleY;

	TexplateTexture(std::string x, std::string pkgsPath) : Texture(x, pkgsPath) {};

};

class TexturePlate : public File
{
private:
	std::string type;
	std::vector<uint32_t> dimensions = { 2048, 2048 };
public:

	void parsePlate();
	void savePlate(std::string fullSavePath);
	std::vector<TexplateTexture*> textures;
	TexturePlate(std::string x, std::string pkgsPath, std::string t) : File(x, pkgsPath)
	{
		type = t;
		parsePlate();
	};
};

class TexturePlateSet : public File
{
private:
	bool dimensionFlag;
	std::vector<TexturePlate*> texplates;
public:
	TexturePlateSet(std::string x, std::string pkgsPath) : File(x, pkgsPath) {};
	bool parse();
	void saveTexturePlateSet(std::string fullSavePath);
};