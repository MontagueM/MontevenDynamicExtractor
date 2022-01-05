#include "texture.h"
//#include <shlwapi.h>

void Texture::getHeader(std::string x)
{
    memcpy((char*)&textureFormat, data + 6, 1);
    memcpy((char*)&width, data + 0x28, 2);
    memcpy((char*)&height, data + 0x2A, 2);
    memcpy((char*)&arraySize, data + 0x2E, 2);
    memcpy((char*)&flag, data + 0x34, 1);
}

void Texture::Get()
{
    dataFile = new File(getReferenceFromHash(hash, packagesPath), packagesPath);
    // Convert type to DXGI_FORMAT format
    bool bCompressed = true;
    switch (textureFormat)
    {
    case 0x30:
        dxgiFormat = DXGI_FORMAT::DXGI_FORMAT_BC1_UNORM;
        bytesPerPixel = 8;
        pixelBlockSize = 4;
        break;
    case 0x40:
        dxgiFormat = DXGI_FORMAT::DXGI_FORMAT_BC2_UNORM;
        bytesPerPixel = 16;
        pixelBlockSize = 4;
        break;
    case 0x50:
        dxgiFormat = DXGI_FORMAT::DXGI_FORMAT_BC3_UNORM;
        bytesPerPixel = 16;
        pixelBlockSize = 4;
        break;
    case 0x60:
        dxgiFormat = DXGI_FORMAT::DXGI_FORMAT_BC4_UNORM;
        bytesPerPixel = 8;
        pixelBlockSize = 4;
        break;
    case 0x70:
        dxgiFormat = DXGI_FORMAT::DXGI_FORMAT_BC5_UNORM;
        bytesPerPixel = 16;
        pixelBlockSize = 4;
        break;
    case 0xA0:
        dxgiFormat = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
        bytesPerPixel = 4;
        pixelBlockSize = 1;
        bCompressed = false;
        break;
    default:
        dxgiFormat = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
        bytesPerPixel = 4;
        pixelBlockSize = 1;
        bCompressed = false;
        break;
    }

    // Large hash
    std::string finalHash = getReferenceFromHash(hash, packagesPath);
    std::string largeHash = getReferenceFromHash(finalHash, packagesPath);
    if (largeHash != "ffffffff" && largeHash != "")
        dataFile = new File(largeHash, packagesPath);
    else
        dataFile = new File(finalHash, packagesPath);

    // Get DirectX uncompressed image
    DirectX::Image DImage;
    DImage.width = width;
    DImage.height = height;
    DImage.format = dxgiFormat;
    size_t rowPitch;
    size_t slicePitch;
    DirectX::ComputePitch(dxgiFormat, width, height, rowPitch, slicePitch);
    DImage.rowPitch = rowPitch;
    DImage.slicePitch = slicePitch;
    size_t fs = dataFile->getData();
    DImage.pixels = considerDoSwizzle(dataFile->data, fs, width, height);
    if (bCompressed)
    {
        DirectX::Decompress(DImage, DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, DSImage);
    }
    else
    {
        DSImage.InitializeFromImage(DImage);
    }
}

static int Morton2D(int t, int sx, int sy)
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
    unsigned char* outData = nullptr;
    if (flag & 0x1)
    {
        outData = new unsigned char[fs];
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
    }
    else
    {
        outData = data;
    }
    return outData;
}

bool Texture::Save(std::string fullSavePath, eTextureFormat TextureFormat)
{
    DirectX::Image DImage = *DSImage.GetImage(0, 0, 0);
    if (!DImage.width) return false;
    std::string FileName;
    std::wstring widestr;
    const wchar_t* widecstr;
    switch (TextureFormat)
    {
    case eTextureFormat::None:
        return false;
    case eTextureFormat::DDS:
        FileName = fullSavePath + hash + ".DDS";
        widestr = std::wstring(FileName.begin(), FileName.end());
        widecstr = widestr.c_str();
        return DirectX::SaveToDDSFile(DImage, DirectX::DDS_FLAGS::DDS_FLAGS_NONE, widecstr);
    case eTextureFormat::TGA:
        FileName = fullSavePath + hash + ".TGA";
        widestr = std::wstring(FileName.begin(), FileName.end());
        widecstr = widestr.c_str();
        return DirectX::SaveToTGAFile(DImage, widecstr);
    case eTextureFormat::PNG:
        FileName = fullSavePath + hash + ".PNG";
        widestr = std::wstring(FileName.begin(), FileName.end());
        widecstr = widestr.c_str();
        return DirectX::SaveToWICFile(DImage, DirectX::WIC_FLAGS::WIC_FLAGS_NONE, GetWICCodec(DirectX::WIC_CODEC_PNG), widecstr);
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

void Material::exportTextures(std::string fullSavePath, eTextureFormat TextureFormat)
{
    std::string actualSavePath;
    std::string newPath;
    for (auto& element : textures)
    {
        uint8_t texID = element.first;
        Texture* tex = element.second;
        if (!tex) continue;
        tex->Get();
        tex->Save(fullSavePath, TextureFormat);
        free(tex);
    }
}