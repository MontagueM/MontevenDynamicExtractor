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

	int maxValue = 0;
	for (auto& tex : textures)
	{
		if (tex->offsetX + tex->scaleX > maxValue)
		{
			maxValue = tex->offsetX + tex->scaleX;
		}
		if (tex->offsetY + tex->scaleY > maxValue)
		{
			maxValue = tex->offsetY + tex->scaleY;
		}
	}

	maxValue = (int)pow(2, ceil(log2(maxValue)));

	DXGI_FORMAT dxFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	if (type == "Diffuse")
		dxFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	DirectX::ScratchImage OutputPlate;
	OutputPlate.Initialize2D(dxFormat, maxValue, maxValue, 1, 0);
	for (auto& tex : textures)
	{
		tex->get();
		DirectX::ScratchImage DSResizedImage;
		DirectX::Rect imageRect = DirectX::Rect(0, 0, tex->width, tex->height);
		DirectX::Resize(*tex->DSImage.GetImage(0, 0, 0), tex->scaleX, tex->scaleY, DirectX::TEX_FILTER_FLAGS::TEX_FILTER_SEPARATE_ALPHA, DSResizedImage);

		DirectX::Image Resized = *DSResizedImage.GetImage(0, 0, 0);
		DirectX::CopyRectangle(Resized, imageRect, *OutputPlate.GetImage(0, 0, 0), DirectX::TEX_FILTER_FLAGS::TEX_FILTER_SEPARATE_ALPHA, tex->offsetX, tex->offsetY);
		
		tex->DSImage.Release();
		DSResizedImage.Release();
		free(tex);
	}

	std::string FileName;
	std::wstring widestr;
	const wchar_t* widecstr;
	FileName = fullSavePath + hash + "_" + type + ".png";
	widestr = std::wstring(FileName.begin(), FileName.end());
	widecstr = widestr.c_str();

	DirectX::SaveToWICFile(*OutputPlate.GetImage(0, 0, 0), DirectX::WIC_FLAGS::WIC_FLAGS_NONE, GetWICCodec(DirectX::WIC_CODEC_PNG), widecstr);

	OutputPlate.Release();
}
