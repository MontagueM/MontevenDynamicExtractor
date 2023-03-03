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
    dxgiFormat = (DXGI_FORMAT)textureFormat;
}

void Texture::save(std::string fullSavePath, std::string saveFormat)
{
    if (DSImage.GetImageCount() == 0) return;
    DirectX::Image DImage = *DSImage.GetImage(0, 0, 0);
    if (!DImage.width) return;
    std::string FileName;
    std::wstring widestr;
    const wchar_t* widecstr;
    if (saveFormat == "png")
    {
        FileName = fullSavePath + ".png";
        widestr = std::wstring(FileName.begin(), FileName.end());
        widecstr = widestr.c_str();
        DirectX::SaveToWICFile(DImage, DirectX::WIC_FLAGS::WIC_FLAGS_NONE, GetWICCodec(DirectX::WIC_CODEC_PNG), widecstr);
    }
    else if (saveFormat == "tga")
    {
        FileName = fullSavePath + ".tga";
        widestr = std::wstring(FileName.begin(), FileName.end());
        widecstr = widestr.c_str();
        DirectX::SaveToTGAFile(DImage, widecstr);
    }
    else
    {
        FileName = fullSavePath + ".dds";
        widestr = std::wstring(FileName.begin(), FileName.end());
        widecstr = widestr.c_str();
		DirectX::SaveToDDSFile(DImage, DirectX::DDS_FLAGS_NONE, widecstr);
	}
}

void Texture::get()
{
    if (largeHash != "ffffffff" && largeHash != "")
        dataFile = new File(largeHash, packagesPath);
    else
        dataFile = new File(getReferenceFromHash(hash, packagesPath), packagesPath);

    dataFile->getData();

    bool bCompressed = false;
    if (70 < textureFormat < 99) bCompressed = true;

    DirectX::Image DImage;

    DImage.width = width;
    DImage.height = height;
	DImage.format = dxgiFormat;

    size_t rowPitch;
    size_t slicePitch;
    DirectX::ComputePitch(dxgiFormat, width, height, rowPitch, slicePitch);
    DImage.rowPitch = rowPitch;
    DImage.slicePitch = slicePitch;
    DImage.pixels = dataFile->data;
    if (bCompressed)
    {
        DirectX::Decompress(DImage, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM, DSImage);
    }
    else
    {
        DSImage.InitializeFromImage(DImage);
    }
    delete dataFile->data;
}

void Material::parseMaterial(std::unordered_map<uint64_t, uint32_t> hash64Table)
{
    uint32_t fileSize;
    fileSize = getData();
    uint32_t textureCount;
    uint32_t textureOffset;
    // Pixel shader textures
    memcpy((char*)&textureCount, data + 0x2B8, 4);
    if (textureCount == 0)
        return;
    memcpy((char*)&textureOffset, data + 0x2C0, 4);
    textureOffset += 0x2C0 + 0x10;
    
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
        else if ((h64Check.substr(h64Check.length() - 2) == "80" || h64Check.substr(h64Check.length() - 2) == "81") && h64Check.substr(h64Check.length() - 4) != "8080")
        {
            std::string textureHash = getReferenceFromHash(h64Check, packagesPath);
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
    std::string newPath;
    for (auto& element : textures)
    {
        uint8_t texID = element.first;
        Texture* tex = element.second;
        newPath = fullSavePath + "/" + tex->hash;
        if (!tex) continue;
        tex->get();
        tex->save(newPath, saveFormat);
        tex->DSImage.Release();
        free(tex);
    }
}

void Material::parseCBuffers()
{
    //beyond light valid stuff

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

    //wq valid stuff

    /*
    90008080 stride 16
    09008080 stride 1
    3F018080 stride 16 external cbuffers
    ---
    vertex 0x70 vertex shader texture offset
    vertex 0x90 09008080 internal offset
    vertex 0xA0 90008080 internal offset
    vertex 0xB0 3F018080 internal offset
    vertex 0xC0 90008080 internal offset
    pixel 0x2D8 09008080 internal offset
    pixel 0x2E8 90008080 internal offset
    pixel 0x2F8 3F018080 internal offset
    pixel 0x308 90008080 internal offset
    pixel 0x30C external cbuffer hash
    */

    std::vector<int> pixelOffsets = { 0x2D8, 0x2E8, 0x308 };
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
    memcpy((char*)&val, data + 0x324, 4);
    if (val != 0xFFFFFFFF)
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
    if (cbType == 0x80800009)
    {
        std::string allFloat = "static float cb" + name + '[' + std::to_string(count) + "] = \n{\n  ";
        allFloat.reserve(count);
        int8_t val;
        for (int i = 0; i < count; i++)
        {
            memcpy((char*)&val, data + offset + i, 1);
            std::string floats = std::to_string((float)val / 128) + ",";
            allFloat += floats;
            if (i % 8 == 0 && i != 0) allFloat += "\n  ";
        }
        allFloat += "\n};\n";
        return allFloat;
    }
    else if (cbType == 0x80800090 || offset == 0)
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
        return "";
    }
}
