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
}