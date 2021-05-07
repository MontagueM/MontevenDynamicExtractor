#include "texplate.h"

bool TexturePlateSet::parse()
{
	int fileSize = getData();
	if (!fileSize) return false;
	uint32_t val;
	memcpy((char*)&val, data + 0x18, 4);
	dimensionFlag = (val == 4);
	TexturePlate* texplate = nullptr;
	// can iterate enum?
	memcpy((char*)&val, data + 0x28, 4);
	texplate = new TexturePlate(uint32ToHexStr(val), packagesPath, "Diffuse");
	texplates.push_back(texplate);
	memcpy((char*)&val, data + 0x2C, 4);
	texplate = new TexturePlate(uint32ToHexStr(val), packagesPath, "Normal");
	texplates.push_back(texplate);
	memcpy((char*)&val, data + 0x30, 4);
	texplate = new TexturePlate(uint32ToHexStr(val), packagesPath, "GStack");
	texplates.push_back(texplate);
	memcpy((char*)&val, data + 0x34, 4);
	// Check to see if we should bother extracting dyemap or not
	texplate = new TexturePlate(uint32ToHexStr(val), packagesPath, "Dyemap");
	texplates.push_back(texplate);
	return true;
}

void TexturePlateSet::saveTexturePlateSet(std::string fullSavePath)
{
	for (auto& plate : texplates)
	{
		plate->savePlate(fullSavePath);
	}
}

void TexturePlate::parsePlate()
{
	int fileSize = getData();
	if (fileSize <= 0x20) return;
	uint32_t count;
	memcpy((char*)&count, data + 0x30, 4);
	uint32_t texHash;
	TexplateTexture* tex = nullptr;
	for (int i = 0x40; i < 0x40 + count * 20; i += 20)
	{
		memcpy((char*)&texHash, data + i, 4);
		tex = new TexplateTexture(uint32ToHexStr(texHash), packagesPath);
		memcpy((char*)&tex->offsetX, data + i + 4, 4);
		memcpy((char*)&tex->offsetY, data + i + 8, 4);
		memcpy((char*)&tex->scaleX, data + i + 0xC, 4);
		memcpy((char*)&tex->scaleY, data + i + 0x10, 4);
		textures.push_back(tex);
	}
}

void TexturePlate::savePlate(std::string fullSavePath)
{
	if (!textures.size()) return;
	if (type == "Dyemap")
	{
		for (auto& val : dimensions)
			val /= 2;
	}
	// Extract every image on plate
	//std::vector<cv::Mat> cvIms;
	cv::Mat4b res(dimensions[0], dimensions[1], cv::Vec4b(0, 0, 0, 0));
	for (auto& tex : textures)
	{
		std::string save = fullSavePath + tex->hash + ".PNG";
		tex->tex2Other(fullSavePath + tex->hash + ".dds", "png");
		cv::Mat cvIm = cv::imread(save, cv::IMREAD_UNCHANGED);
		if (cvIm.empty())
		{
			printf("Tex not written!");
			exit(1);
		}
		cvIm.copyTo(res(cv::Rect(tex->offsetX, tex->offsetY, tex->scaleX, tex->scaleY)));
		remove(save.c_str());
		free(tex);
	}
	cv::imwrite(fullSavePath + hash + "_" + type + ".png", res);
}