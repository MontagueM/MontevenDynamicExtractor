#include <cstdint>
#include "dynamic.h"
extern "C"
{
	__declspec(dllexport) bool __stdcall RequestDynamicInformation(const char* DynamicHash, const char* pkgsPath, int& MeshCount, bool& bHasSkeleton)
	{
		Dynamic dynamic = Dynamic(DynamicHash, pkgsPath);
		int status = dynamic.RequestInfo(MeshCount, bHasSkeleton);
		if (status)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	__declspec(dllexport) bool __stdcall RequestSaveDynamicMeshData(const char* DynamicHash, const char* pkgsPath)
	{
		Dynamic dynamic = Dynamic(DynamicHash, pkgsPath);
		int status = dynamic.RequestSaveDynamicMeshData();
		if (status)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	__declspec(dllexport) bool __stdcall RequestExportDynamic(const char* DynamicHash, const char* pkgsPath, const char* ExportPath, const char* ExportName, bool bTextures)
	{
		std::unordered_map<uint64_t, uint32_t> hash64Table;
		std::ifstream f("h64");
		if (f)
		{
			hash64Table = loadH64Table();
			if (hash64Table.size() < 10000)
			{
				hash64Table = generateH64Table(pkgsPath);
				saveH64Table(hash64Table);
			}
		}
		else
		{
			hash64Table = generateH64Table(pkgsPath);
			saveH64Table(hash64Table);
		}

		Dynamic dynamic = Dynamic(DynamicHash, hash64Table, pkgsPath, bTextures, -1);
		int status = dynamic.get();
		if (status)
		{
			dynamic.pack(ExportPath, false, "png");
			dynamic.save(ExportPath, ExportName);
			return true;
		}
		else
		{
			return false;
		}
	}
}