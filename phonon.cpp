#include <cstdint>
#include "dynamic.h"
#include "d1map.h"
extern "C"
{


	__declspec(dllexport) bool __stdcall RequestDynamicInformation(const char* DynamicHash, const char* pkgsPath, int& MeshCount, bool& bHasSkeleton)
	{
		Dynamic* dynamic = new Dynamic(DynamicHash, pkgsPath);
		int status = dynamic->RequestInfo(MeshCount, bHasSkeleton);
		delete dynamic;
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
		Dynamic* dynamic = new Dynamic(DynamicHash, pkgsPath);
		int status = dynamic->RequestSaveDynamicMeshData();
		delete dynamic;
		if (status)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	__declspec(dllexport) bool __stdcall RequestExportDynamic(const char* DynamicHash, const char* pkgsPath, const char* ExportPath, const char* ExportName, int TextureFormat)
	{
		Dynamic* dynamic = new Dynamic(DynamicHash, pkgsPath, eTextureFormat(TextureFormat));
		int status = dynamic->get();
		if (status)
		{
			dynamic->pack(ExportPath);
			dynamic->save(ExportPath, ExportName);
			delete dynamic;
			return true;
		}
		else
		{
			delete dynamic;
			return false;
		}
	}

	__declspec(dllexport) bool __stdcall RequestExportD1Map(const char* MapHash, const char* pkgsPath, const char* ExportPath, const char* ExportName, int TextureFormat)
	{
		D1Map* d1map = new D1Map(MapHash, pkgsPath, eTextureFormat(TextureFormat));
		int status = d1map->Get();
		if (status)
		{
			d1map->Extract(ExportPath, ExportName);
			delete d1map;
			return true;
		}
		else
		{
			delete d1map;
			return false;
		}
	}

	__declspec(dllexport) bool _stdcall RequestExportD1DynMap(const char* MapHash, const char* pkgsPath, const char* ExportPath, const char* ExportName)
	{
		D1DynMap* d1map = new D1DynMap(MapHash, pkgsPath);
		int status = d1map->Get();
		if (status)
		{
			d1map->Extract(ExportPath, ExportName);
			delete d1map;
			return true;
		}
		else
		{
			delete d1map;
			return false;
		}
	}
}