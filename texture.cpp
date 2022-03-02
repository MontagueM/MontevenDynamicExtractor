#include "texture.h"
//#include <shlwapi.h>
//#pragma comment(lib, "shlwapi.lib")

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

void Material::parseMaterial(std::unordered_map<uint64_t, uint32_t> hash64Table)
{
    getData();
    uint32_t textureCount;
    uint32_t textureOffset;
    // Pixel shader textures
    memcpy((char*)&textureCount, data + 0x2B8, 4);
    //memcpy((char*)&textureOffset, data + 0x2A8, 4);
    //textureOffset += 0x2A8 + 0x10;
    //Reading TextureOffset from 0x2A8 or 0x2D8 (my initial fix) is now unreliable for now afaik
    //so this solution is quick, dirty, and probably slower but it works ig

    int extOff = fs - 16;
    bool bFound = false;
    uint32_t val;
    while (true)
    {
        memcpy((char*)&val, data + extOff, 4);
        if (val == 0x80806DCF)
        {
            bFound = true;
            extOff += 8;
            textureOffset = extOff;
            break;
        }
        extOff -= 4;
    }
    if (!bFound) {
        return;
    }


    uint64_t h64Val;
    for (int i = textureOffset; i < textureOffset + textureCount * 0x18; i += 0x18)
    {
        uint8_t textureIndex;
        memcpy((char*)&textureIndex, data + i, 1);
        uint32_t val;
        memcpy((char*)&val, data + i + 8, 4);
        std::string h64Check = uint32ToHexStr(val);
        if (h64Check == "ffffffff")
        {
            memcpy((char*)&h64Val, data + i + 0x10, 8);
            if (h64Val == 0) continue;
            std::string textureHash = getHash64(h64Val, hash64Table);
            if (textureHash != "ffffffff")
            {
                Texture* texture = new Texture(textureHash, packagesPath);
                textures[textureIndex] = texture;
            }
        }
        else if (h64Check.substr(h64Check.length() - 2) == "80" && h64Check.substr(h64Check.length() - 4) != "8080")
        {
            std::string textureHash = getReferenceFromHash(h64Check, packagesPath);
            //std::cout << textureHash + "\n"; debugging nonsense
            Texture* texture = new Texture(textureHash, packagesPath);
            textures[textureIndex] = texture;
        }
	else
	{
	    printf("Support old texture format");
	    return;
	}
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

void Material::parseCBuffers()
{
    /*
    90008080 stride 16
    09008080 stride 1
    3F018080 stride 16 external cbuffers
    ---
    vertex 0x68 vertex shader texture offset
    vertex 0x80 09008080 internal offset
    vertex 0x90 90008080 internal offset
    vertex 0xA0 3F018080 internal offset
    vertex 0xB0 90008080 internal offset
    pixel 0x2C0 09008080 internal offset
    pixel 0x2E0 3F018080 internal offset
    pixel 0x2D0 90008080 internal offset
    pixel 0x2F0 90008080 internal offset
    pixel 0x30C external cbuffer hash
    */
    std::vector<int> pixelOffsets = { 0x2C0, 0x2D0, 0x2F0 };
    uint32_t val;
    uint32_t count;
    getData();
    int i = 0;
    for (auto& offset : pixelOffsets)
    {
        memcpy((char*)&val, data + offset, 4);
        if (val == 0) continue;
        offset += val;
        memcpy((char*)&count, data + offset, 4);
        uint32_t cbType;
        memcpy((char*)&cbType, data + offset + 8, 4);
        std::string floats = getCBufferFromOffset(data, offset + 0x10, count, cbType, std::to_string(i));
        cbuffers.push_back(floats);
        i++;
    }
    memcpy((char*)&val, data + 0x30C, 4);
    if (val != 4294967295)
    {
        File buffer = File(getReferenceFromHash(uint32ToHexStr(val), packagesPath), packagesPath);
        int fileSize = buffer.getData();
        std::string pixelExternalFloats = getCBufferFromOffset(buffer.data, 0, fileSize / 16, 1, "pixelExt");
        cbuffers.push_back(pixelExternalFloats);
    }
}

void Material::writeCBuffers(std::string fullSavePath)
{
    parseCBuffers();
    FILE* cbFile;
    std::string path = fullSavePath + "/cbuffers.txt";
    fopen_s(&cbFile, path.c_str(), "a");
    std::string header = "\n--------\nCBUFFERS FOR MATERIAL " + hash + ":\n--------\n";
    fwrite(header.c_str(), header.size(), 1, cbFile);
    for (auto& cbuffer : cbuffers)
    {
        fwrite(cbuffer.c_str(), cbuffer.size(), 1, cbFile);
    }
    fclose(cbFile);
}

std::string getCBufferFromOffset(unsigned char* data, int offset, int count, uint32_t cbType, std::string name)
{
    if (cbType == 2155872265)
    {
        std::string allFloat = "static float cb" + name + '[' + std::to_string(count) + "] = \n{\n  ";
        allFloat.reserve(count);
        int8_t val;
        for (int i = 0; i < count; i++)
        {
            memcpy((char*)&val, data + offset + i, 1);
            std::string floats = std::to_string((float)val/128) + ",";
            allFloat += floats;
            if (i % 8 == 0 && i != 0) allFloat += "\n  ";
        }
        allFloat += "\n};\n";
        return allFloat;
    }
    else if (cbType == 2155872400 || offset == 0)
    {
        std::string allFloat4 = "static float4 cb" + name + '[' + std::to_string(count) + "] = \n{\n";
        float_t val;
        for (int i = 0; i < count; i++)
        {
            std::string float4 = "  float4(";
            for (int j = 0; j < 4; j++)
            {
                memcpy((char*)&val, data + offset + i * 16 + j * 4, 4);
                float4 += std::to_string(val);
                if (j < 3) float4 += ", ";
                else float4 += "),\n";
            }
            allFloat4 += float4;
        }
        allFloat4 += "};\n";
        return allFloat4;
    }
    else
    {
        std::cerr << "\nUnknown cbuffer class found, exiting...\n";
        exit(1);
    }
}
