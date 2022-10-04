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

void TexturePlateSet::saveTexturePlateSet(std::string fullSavePath, std::string saveFormat)
{
	for (auto& plate : texplates)
	{
		plate->savePlate(fullSavePath, saveFormat);
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

void TexturePlate::savePlate(std::string fullSavePath, std::string saveFormat)
{
	if (!textures.size()) return;

	if (type == "Dyemap")
	{
		for (auto& val : dimensions)
			val /= 2;
	}

	// See if we need to shrink the texture by half
	int maxValue = 0;
	for (auto& tex : textures)
	{
		if (tex->offsetX + tex->scaleX > maxValue)
		{
			maxValue = tex->offsetX + tex->scaleX;
		}
		else if (tex->offsetY + tex->scaleY > maxValue)
		{
			maxValue = tex->offsetY + tex->scaleY;
		}
	}
	if (maxValue <= dimensions[0] / 2 && maxValue <= dimensions[1] / 2)
	{
		for (auto& val : dimensions)
			val /= 2;
	}

	// Extract every image on plate
	DirectX::ScratchImage OutputPlate;
	OutputPlate.Initialize2D(DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM, dimensions[0], dimensions[1], 1, 0);
	for (auto& tex : textures)
	{
		tex->get();
		DirectX::Rect ImageRect(0, 0, tex->width, tex->height);
		DirectX::ScratchImage DSResizedImage;
		DirectX::Resize(*tex->DSImage.GetImage(0,0,0), tex->scaleX, tex->scaleY,
			DirectX::TEX_FILTER_FLAGS::TEX_FILTER_DEFAULT, DSResizedImage);

		DirectX::Image Resized1 = *DSResizedImage.GetImage(0, 0, 0);
		DirectX::CopyRectangle(*DSResizedImage.GetImage(0, 0, 0), ImageRect, *OutputPlate.GetImage(0, 0, 0), DirectX::TEX_FILTER_FLAGS::TEX_FILTER_DEFAULT, tex->offsetX, tex->offsetY);
		tex->DSImage.Release();
		DSResizedImage.Release();
		free(tex);
	}
	
	std::string FileName;
	std::wstring widestr;
	const wchar_t* widecstr;
	if (saveFormat == "dds")
	{
		FileName = fullSavePath + hash + "_" + type + ".DDS";
		widestr = std::wstring(FileName.begin(), FileName.end());
		widecstr = widestr.c_str();
		DirectX::SaveToDDSFile(*OutputPlate.GetImage(0, 0, 0), DirectX::DDS_FLAGS::DDS_FLAGS_NONE, widecstr);
	}
	else if (saveFormat == "tga")
	{
		FileName = fullSavePath + hash + "_" + type + ".TGA";
		widestr = std::wstring(FileName.begin(), FileName.end());
		widecstr = widestr.c_str();
		DirectX::SaveToTGAFile(*OutputPlate.GetImage(0, 0, 0), widecstr);
	}
	else if (saveFormat == "png")
	{
		FileName = fullSavePath + hash + "_" + type + ".PNG";
		widestr = std::wstring(FileName.begin(), FileName.end());
		widecstr = widestr.c_str();
		DirectX::SaveToWICFile(*OutputPlate.GetImage(0, 0, 0), DirectX::WIC_FLAGS::WIC_FLAGS_NONE,
			GetWICCodec(DirectX::WIC_CODEC_PNG), widecstr);
	}
	OutputPlate.Release();
}