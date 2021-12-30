#include "texture.h"
//#include <shlwapi.h>
//#pragma comment(lib, "shlwapi.lib")

void Texture::getHeader(std::string x)
{
    memcpy((char*)&textureFormat, data + 6, 1);
    memcpy((char*)&width, data + 0x28, 2);
    memcpy((char*)&height, data + 0x2A, 2);
    memcpy((char*)&arraySize, data + 0x2E, 2);
}

void Texture::tex2DDS(std::string fullSavePath)
{
    dataFile = new File(getReferenceFromHash(hash, packagesPath), packagesPath);
    // Convert type to DXGI_FORMAT format
    bool bCompressed = true;
    switch (textureFormat)
    {
    case 0x30:
        dxgiFormat = "BC1_UNORM";
        bytesPerPixel = 8;
        pixelBlockSize = 4;
        break;
    case 0x40:
        dxgiFormat = "BC2_UNORM";
        bytesPerPixel = 16;
        pixelBlockSize = 4;
        break;
    case 0x50:
        dxgiFormat = "BC3_UNORM";
        bytesPerPixel = 16;
        pixelBlockSize = 4;
        break;
    case 0x60:
        dxgiFormat = "BC4_UNORM";
        bytesPerPixel = 8;
        pixelBlockSize = 4;
        break;
    case 0x70:
        dxgiFormat = "BC5_UNORM";
        bytesPerPixel = 16;
        pixelBlockSize = 4;
        break;
    case 0xA0:
        dxgiFormat = "R8G8B8A8_UNORM";
        bytesPerPixel = 4;
        pixelBlockSize = 1;
        bCompressed = false;
        break;
    default:
        dxgiFormat = "R8G8B8A8_UNORM";
        bytesPerPixel = 4;
        pixelBlockSize = 1;
        bCompressed = false;
        break;
    }
    auto itr = std::find(DXGI_FORMAT.begin(), DXGI_FORMAT.end(), dxgiFormat);
    textureFormat = std::distance(DXGI_FORMAT.begin(), itr);

    // Large hash
    std::string finalHash = getReferenceFromHash(hash, packagesPath);
    std::string largeHash = getReferenceFromHash(finalHash, packagesPath);
    if (largeHash != "ffffffff" && largeHash != "")
        dataFile = new File(largeHash, packagesPath);
    else
        dataFile = new File(finalHash, packagesPath);

    writeTexture(fullSavePath, dxgiFormat, bCompressed);
}

void Texture::tex2Other(std::string fullSavePath, std::string saveFormat)
{
    tex2DDS(fullSavePath);

    std::string str = "texconv.exe \"" + fullSavePath + "\" -y -ft " + saveFormat + " -f " + dxgiFormat;
    printf(str.c_str());
    system(str.c_str());

    // Delete dds file if it exists
    std::string newPath = fullSavePath.substr(0, fullSavePath.size() - 3) + saveFormat;
    std::ifstream f(newPath);
    if (f) std::remove(fullSavePath.c_str());
}

void Texture::writeTexture(std::string fullSavePath, std::string dxgiFormat, bool bCompressed)
{
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

int Morton2D(int t, int sx, int sy)
{
    int num2 = 0;
    int num = num2 = 1;
    int num3 = t;
    int num4 = sx;
    int num5 = sy;
    int num6 = 0;
    int num7 = 0;
    while (num4 > 1 || num5 > 1)
    {
        if (num4 > 1)
        {
            num6 += num2 * (num3 & 1);
        }
        num3 >>= 1;
        num2 *= 2;
        num4 >>= 1;
        if (num5 > 1)
        {
            num7 += num * (num3 & 1);
        }
        num3 >>= 1;
        num *= 2;
        num5 >>= 1;
    }
    return num7 * sx + num6;
}

unsigned char* Texture::considerDoSwizzle(unsigned char* data, int fs, int width, int height)
{
        unsigned char* outData = new unsigned char[fs];
        int ptr = 0;
        int tHeight = height / pixelBlockSize;
        int tWidth = width / pixelBlockSize;
        for (int y = 0; y < (tHeight + 7) / 8; y++)
        {
            for (int x = 0; x < (tWidth + 7) / 8; x++)
            {
                for (int k = 0; k < 64; k++)
                {
                    int pixelIndex = Morton2D(k, 8, 8);
                    int xOffset = (x * 8) + (pixelIndex % 8);
                    int yOffset = (y * 8) + (pixelIndex / 8);
                    if (xOffset < tWidth && yOffset < tHeight)
                    {
                        int idx = bytesPerPixel * (yOffset * tWidth + xOffset);
                        memcpy(outData + idx, data + ptr, bytesPerPixel);
                        ptr += bytesPerPixel;
                    }

                }
            }
        }
        return outData;
}

void Texture::writeFile(DDSHeader dds, DXT10Header dxt, std::string fullSavePath)
{
    FILE* outputFile;

    fopen_s(&outputFile, fullSavePath.c_str(), "wb");
    if (outputFile != NULL) {
        fwrite(&dds, sizeof(struct DDSHeader), 1, outputFile);
        fwrite(&dxt, sizeof(struct DXT10Header), 1, outputFile);
        int fileSize = dataFile->getData();
        fwrite(considerDoSwizzle(dataFile->data, fileSize, width, height), fileSize, 1, outputFile);
        fclose(outputFile);
    }
}

void Material::parseMaterial()
{
    getData();
    uint32_t textureCount;
    uint32_t textureOffset;
    // Pixel shader textures
    memcpy((char*)&textureCount, data + 0x2B8, 4);
    memcpy((char*)&textureOffset, data + 0x2C0, 4);
    textureOffset += 0x2C0 + 0x10;;
    for (int i = textureOffset; i < textureOffset + textureCount * 0x8; i += 0x8)
    {
        uint8_t textureIndex;
        memcpy((char*)&textureIndex, data + i, 1);
        uint32_t val;
        memcpy((char*)&val, data + i + 4, 4);
        std::string h = uint32ToHexStr(val);
        if (h == "ffffffff") continue;
        Texture* texture = new Texture(h, packagesPath);
        textures[textureIndex] = texture;
    }
}

void Material::exportTextures(std::string fullSavePath, std::string saveFormat)
{
    std::string actualSavePath;
    std::string newPath;
    for (auto& element : textures)
    {
        uint8_t texID = element.first;
        Texture* tex = element.second;
        actualSavePath = fullSavePath + "/" + tex->hash + ".dds";
        newPath = fullSavePath + "/" + tex->hash + "." + saveFormat;
        std::ifstream f(newPath);
        std::ifstream q(actualSavePath);
        if (f || q)
        {
            free(tex);
            continue;
        }
        if (saveFormat == "dds") tex->tex2DDS(actualSavePath);
        else tex->tex2Other(actualSavePath, saveFormat);
        free(tex);
    }
}