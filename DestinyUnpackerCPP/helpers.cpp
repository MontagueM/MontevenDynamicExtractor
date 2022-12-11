#include "helpers.h"

std::string uint16ToHexStr(uint16_t num)
{
	std::stringstream stream;
	stream << std::hex << num;
	std::string hexStr = stream.str();
	if (hexStr.size() % 4 != 0)
		hexStr = std::string(4 - (hexStr.size() % 4), '0').append(hexStr);
	return hexStr;
}

std::string uint32ToHexStr(uint32_t num)
{
	std::stringstream stream;
	stream << std::hex << swapUInt32Endianness(num);
	std::string hexStr = stream.str();
	if (hexStr.size() % 8 != 0)
		hexStr = std::string(8 - (hexStr.size() % 8), '0').append(hexStr);
	return hexStr;
}

uint32_t hexStrToUint16(std::string hash)
{
	return swapUInt16Endianness(std::stoul(hash, nullptr, 16));
}

uint32_t hexStrToUint32(std::string hash)
{
	return swapUInt32Endianness(std::stoul(hash, nullptr, 16));
}

uint64_t hexStrToUint64(std::string hash)
{
	return swapUInt64Endianness(std::stoull(hash, nullptr, 16));
}

uint16_t swapUInt16Endianness(uint16_t x)
{
	x = (x << 8) + (x >> 8);
	return x;
}

uint32_t swapUInt32Endianness(uint32_t x)
{
	x = (x >> 24) |
		((x << 8) & 0x00FF0000) |
		((x >> 8) & 0x0000FF00) |
		(x << 24);
	return x;
}

uint64_t swapUInt64Endianness(uint64_t k)
{
	return ((k << 56) |
		((k & 0x000000000000FF00) << 40) |
		((k & 0x0000000000FF0000) << 24) |
		((k & 0x00000000FF000000) << 8) |
		((k & 0x000000FF00000000) >> 8) |
		((k & 0x0000FF0000000000) >> 24) |
		((k & 0x00FF000000000000) >> 40) |
		(k >> 56)
		);
}