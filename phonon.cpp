#include <cstdint>
#include "dynamic.h"
#include "d1map.h"
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
		Dynamic dynamic = Dynamic(DynamicHash, pkgsPath, bTextures);
		int status = dynamic.get();
		if (status)
		{
			dynamic.pack(ExportPath);
			dynamic.save(ExportPath, ExportName);
			return true;
		}
		else
		{
			return false;
		}
	}

	__declspec(dllexport) bool __stdcall RequestExportD1Map(const char* MapHash, const char* pkgsPath, const char* ExportPath, const char* ExportName, bool bTextures)
	{
		D1Map d1map = D1Map(MapHash, pkgsPath, bTextures);
		int status = d1map.Get();
		if (status)
		{
			d1map.Extract(ExportPath, ExportName);
			return true;
		}
		else
		{
			return false;
		}

	}
}