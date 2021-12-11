#include "texture.h"
//#include <shlwapi.h>
//#pragma comment(lib, "shlwapi.lib")

void Texture::getHeader(std::string x)
{
    memcpy((char*)&textureFormat, data + 4, 2);
    memcpy((char*)&width, data + 0x0E, 2);
    memcpy((char*)&height, data + 0x10, 2);
    memcpy((char*)&arraySize, data + 0x14, 2);
    uint32_t val;
    memcpy((char*)&val, data + 0x24, 4);
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

void Texture::tex2Other(std::string fullSavePath, std::string saveFormat)
{
    tex2DDS(fullSavePath);
    std::string dxgiFormat;
    dxgiFormat = DXGI_FORMAT[textureFormat];

    /// Code to try and fix texconv not always working from command line, couldn't get it to work
    //wchar_t exePath[MAX_PATH];
    //DWORD nSize = 0;
    //GetModuleFileName(NULL, exePath, MAX_PATH);
    //std::wstring wPath(exePath);
    //wPath.erase(wPath.rfind('\\'));

    //std::wstring wFSP(fullSavePath.begin(), fullSavePath.end());
    //std::wstring wsaveFormat(saveFormat.begin(), saveFormat.end());
    //std::wstring wdxgiFormat(dxgiFormat.begin(), dxgiFormat.end());

    //std::wstring wPathNoBackslashes = L"";

    //for (auto& c : wPath)
    //{
    //    if (c == '\\') wPathNoBackslashes += '/';
    //    else wPathNoBackslashes += c;
    //}

    //std::wstring str = L'"' + wPathNoBackslashes + L"/texconv.exe" + L'"' + L" " + wFSP + L"\" -y -ft " + wsaveFormat + L" -f " + wdxgiFormat;
    //wprintf(str.c_str());
    //_wsystem(str.c_str());

    std::string str = "texconv.exe \"" + fullSavePath + "\" -y -ft " + saveFormat + " -f " + dxgiFormat;
    printf(str.c_str());
    system(str.c_str());

    // Delete dds file if it exists
    std::string newPath = fullSavePath.substr(0, fullSavePath.size() - 3) + saveFormat;
    std::ifstream f(newPath);
    if (f) std::remove(fullSavePath.c_str());
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
    // Pixel shader textures
    memcpy((char*)&textureCount, data + 0x2D0, 4);
    memcpy((char*)&textureOffset, data + 0x2D8, 4);
    textureOffset += 0x2D8 + 0x10;;
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