#include "texture.h"

void Texture::getHeader(std::string x)
{
	memcpy((char*)&textureFormat, data + 4, 2);
	memcpy((char*)&width, data + 0x22, 2);
	memcpy((char*)&height, data + 0x24, 2);
	memcpy((char*)&arraySize, data + 0x28, 2);
	uint32_t val;
	memcpy((char*)&val, data + 0x3C, 4);
	largeHash = uint32ToHexStr(val);
}

void Texture::tex2DDS(std::string fullSavePath)
{
	if (largeHash != "ffffffff" && largeHash != "")
		dataFile = new File(largeHash, packagesPath);
	else
		dataFile = new File(getReferenceFromHash(hash, packagesPath), packagesPath);
	writeTexture(fullSavePath);
}

void Texture::writeTexture(std::string fullSavePath)
{
	bool bCompressed = false;
	if (70 < textureFormat < 99) bCompressed = true;

	DDSHeader dds;
    DXT10Header dxt;
    dds.MagicNumber = 542327876;
    dds.dwSize = 124;
    dds.dwFlags = (0x1 + 0x2 + 0x4 + 0x1000) + 0x8;
    dds.dwHeight = height;
    dds.dwWidth = width;
    dds.dwPitchOrLinearSize = 0;
    dds.dwDepth = 0;
    dds.dwMipMapCount = 0;
    dds.dwReserved1 = std::array<uint32_t, 11>();
    dds.dwPFSize = 32;
    dds.dwPFRGBBitCount = 0;
    dds.dwPFRGBBitCount = 32;
    dds.dwPFRBitMask = 0xFF;
    dds.dwPFGBitMask = 0xFF00;
    dds.dwPFBBitMask = 0xFF0000;
    dds.dwPFABitMask = 0xFF000000;
    dds.dwCaps = 0x1000;
    dds.dwCaps2 = 0;
    dds.dwCaps3 = 0;
    dds.dwCaps4 = 0;
    dds.dwReserved2 = 0;
    if (bCompressed)
    {
        dds.dwPFFlags = 0x1 + 0x4;  // contains alpha data + contains compressed RGB data
        dds.dwPFFourCC = 808540228;
        dxt.dxgiFormat = textureFormat;
        dxt.resourceDimension = 3;  // DDS_DIMENSION_TEXTURE2D
        if (arraySize % 6 == 0)
        {
            // Compressed cubemap
            dxt.miscFlag = 4;
            dxt.arraySize = arraySize / 6;
        }
        else
        {
            // Compressed BCn
            dxt.miscFlag = 0;
            dxt.arraySize = 1;
        }
    }
    else
    {
        // Uncompressed
        dds.dwPFFlags = 0x1 + 0x40;  // contains alpha data + contains uncompressed RGB data
        dds.dwPFFourCC = 0;
        dxt.miscFlag = 0;
        dxt.arraySize = 1;
        dxt.miscFlags2 = 0x1;
    }

    writeFile(dds, dxt, fullSavePath);
}

void Texture::writeFile(DDSHeader dds, DXT10Header dxt, std::string fullSavePath)
{
    FILE* outputFile;
    
    fopen_s(&outputFile, fullSavePath.c_str(), "wb");
    if (outputFile != NULL) {
        fwrite(&dds, sizeof(struct DDSHeader), 1, outputFile);
        fwrite(&dxt, sizeof(struct DXT10Header), 1, outputFile);
        int fileSize = dataFile->getData();
        fwrite(dataFile->data, fileSize, 1, outputFile);
        fclose(outputFile);
    }
}

void Material::parseMaterial()
{
    getData();
    uint32_t textureCount;
    uint32_t textureOffset;
    memcpy((char*)&textureCount, data + 0x2A0, 4);
    memcpy((char*)&textureOffset, data + 0x2A8, 4);
    textureOffset += 0x2A8 + 0x10;

    for (int i = textureOffset; i < textureOffset + textureCount * 0x18; i += 0x18)
    {
        uint32_t textureIndex;
        memcpy((char*)&textureIndex, data + i, 4);
        uint32_t val;
        memcpy((char*)&val, data + i + 8, 4);
        std::string h64Check = uint32ToHexStr(val);
        if (h64Check == "ffffffff")
        {
            std::string hash64 = getHash64(hash, packagesPath);
        }
    }
}

void Material::exportTextures(std::string fullSavePath, std::string saveFormat)
{

}