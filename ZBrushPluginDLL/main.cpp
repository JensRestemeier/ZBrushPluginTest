#include "main.h"

#include "surface.h"

#include <Windows.h>
#include <delayimp.h>
#include <map>
#include <vector>
#include <cstring>
#include <zlib.h>
#include <pathcch.h>

typedef std::map<int, Surface*> SurfaceMap;
typedef std::vector<int> SurfaceIdStack;
SurfaceMap surfaceMap;
SurfaceIdStack surfaceIdStack;
int surfaceCount = 0;

static Surface::Channel mapChannel(const char * pStrValue)
{
	if (strcmp(pStrValue, "RGB") == 0) {
		return Surface::RGB;
	}
	else if (strcmp(pStrValue, "D") == 0) {
		return Surface::D;
	}
	else if (strcmp(pStrValue, "M") == 0) {
		return Surface::M;
	}
	else if (strcmp(pStrValue, "NX") == 0) {
		return Surface::NX;
	}
	else if (strcmp(pStrValue, "NY") == 0) {
		return Surface::NY;
	}
	else if (strcmp(pStrValue, "NZ") == 0) {
		return Surface::NZ;
	}
	return Surface::Unknown;
}

static int getSurfaceId()
{
	int surfaceId;
		if (!surfaceIdStack.empty())
	{
			surfaceId = surfaceIdStack.back();
			surfaceIdStack.pop_back();
	}
		else
		{
			surfaceId = surfaceCount++;
		}
		return surfaceId;
}

static void freeSurfaceId(int surfaceId)
{
	if (surfaceId + 1 == surfaceCount)
	{
		surfaceCount--;
	}
	else
	{
		surfaceIdStack.push_back(surfaceId);
	}
}

// Get the version number from the DLL resource. This makes sure the version number always matches between the 
// runtime and whatever the file properties show.
ZBRUSH_API float Version(char* pDontCare, double optValue, char* pOptBuffer1, int optBuffer1Size, char* pOptBuffer2, int optBuffer2Size, char** zData)
{
	// get the path for the current DLL
	HMODULE hm = NULL;
	if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPWSTR)&Version, &hm) == 0)
	{
		return -1.0f;
	}
	WCHAR path[MAX_PATH];
	if (GetModuleFileName(hm, path, sizeof(path)) == 0)
	{
		return -1.0f;
	}

	// Get the version information for the file requested
	DWORD dwSize = GetFileVersionInfoSize(path, NULL);
	if (dwSize == 0)
	{
		return -1.0f;
	}

	BYTE *pbVersionInfo = new BYTE[dwSize];
	if (!GetFileVersionInfo(path, 0, dwSize, pbVersionInfo))
	{
		delete[] pbVersionInfo;
		return -1.0f;
	}

	VS_FIXEDFILEINFO *fileInfo = NULL;
	UINT fileInfoLen = 0;
	if (!VerQueryValue(pbVersionInfo, TEXT("\\"), (LPVOID*)&fileInfo, &fileInfoLen))
	{
		delete[] pbVersionInfo;
		return -1.0f;
	}

	float version = (float)HIWORD(fileInfo->dwFileVersionMS) + (float)LOWORD(fileInfo->dwFileVersionMS) / 100.0f;

	delete[] pbVersionInfo;

	return version;
}

ZBRUSH_API float Initialise(char* pStrValue, double optValue, char* outputBuffer, int optBuffer1Size,
	char* pOptBuffer2, int optBuffer2Size, char** zData)
{
	HMODULE hm = NULL;
	if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)Initialise, &hm) == 0)
	{
		int ret = GetLastError();
		char buf[256];
		sprintf(buf, "GetModuleHandle failed, error = %d\n", ret);
		MessageBoxA(0, buf, "DLL initialisation error", MB_OK | MB_ICONERROR);
		return 0.0f;
	}

	TCHAR path[MAX_PATH];
	if (GetModuleFileName(hm, path, sizeof(path)) == 0)
	{
		int ret = GetLastError();
		char buf[256];
		sprintf(buf, "GetModuleFileName failed, error = %d\n", ret);
		MessageBoxA(0, buf, "DLL initialisation error", MB_OK | MB_ICONERROR);
		return 0.0f;
	}

	PathCchRemoveFileSpec(path, MAX_PATH);

	// DLL loads fail without the plugin directory added to the DLL search path...
	SetDllDirectory(path);

	return 0.0f;
}

ZBRUSH_API float HelloDLL(char* pStrValue, double optValue, char* outputBuffer, int optBuffer1Size,
							char* pOptBuffer2, int optBuffer2Size, char** zData)
{
    MessageBoxA(0, pStrValue, "DLL received Message", MB_OK | MB_ICONINFORMATION);
	strcpy(outputBuffer,"Hello from my DLL");

    return 0.0f;
}

ZBRUSH_API float CreateSurface(char* pStrValue, double optValue, char* pOptBuffer1, int optBuffer1Size, char* pOptBuffer2, int optBuffer2Size, char** zData)
{
	// extract size from surface
	char buf[256];
	strncpy(buf, pStrValue, sizeof(buf) - 1);
	buf[255] = 0;
	char * comma = strchr(buf, ',');
	*comma = 0;
	int width = strtol(buf, NULL, 10);
	int height = strtol(comma + 1, NULL, 10);

	int surfaceId = getSurfaceId();
	surfaceMap[surfaceId] = new Surface(width, height);
	return (float)surfaceId;
}

ZBRUSH_API float DestroySurface(char* pStrValue, double optValue, char* pOptBuffer1, int optBuffer1Size, char* pOptBuffer2, int optBuffer2Size, char** zData)
{
	int surfaceId = (int)optValue;
	SurfaceMap::iterator i = surfaceMap.find(surfaceId);
	if (i != surfaceMap.end())
	{
		delete i->second;
		surfaceMap.erase(i);
		freeSurfaceId(surfaceId);
	}
	return 0.0f;
}

ZBRUSH_API float ReadSurface(char* pStrValue, double optValue, char* pOptBuffer1, int optBuffer1Size, char* pOptBuffer2, int optBuffer2Size, char** zData)
{
	int surfaceId = getSurfaceId();
	surfaceMap[surfaceId] = Surface::load(pStrValue);
	return (float)surfaceId;
}

ZBRUSH_API float WriteSurface(char* pStrValue, double optValue, char* pOptBuffer1, int optBuffer1Size, char* pOptBuffer2, int optBuffer2Size, char** zData)
{
	SurfaceMap::iterator i = surfaceMap.find((int)optValue);
	if (i != surfaceMap.end())
	{
		i->second->save(pStrValue);
	}
	return 0.0f;
}

ZBRUSH_API float GetWidth(char* pStrValue, double optValue, char* pOptBuffer1, int optBuffer1Size, char* pOptBuffer2, int optBuffer2Size, char** zData)
{
	SurfaceMap::iterator i = surfaceMap.find((int)optValue);
	if (i != surfaceMap.end())
	{
		return (float) i->second->getWidth();
	}
	return -1.0f;
}

ZBRUSH_API float GetHeight(char* pStrValue, double optValue, char* pOptBuffer1, int optBuffer1Size, char* pOptBuffer2, int optBuffer2Size, char** zData)
{
	SurfaceMap::iterator i = surfaceMap.find((int)optValue);
	if (i != surfaceMap.end())
	{
		return (float)i->second->getHeight();
	}
	return -1.0f;
}

ZBRUSH_API float SetChannel(char* pStrValue, double optValue, char* pOptBuffer1, int optBuffer1Size, char* pOptBuffer2, int optBuffer2Size, char** zData)
{
	SurfaceMap::iterator i = surfaceMap.find((int)optValue);
	if (i != surfaceMap.end())
	{
		i->second->setChannel(mapChannel(pStrValue), pOptBuffer1, optBuffer1Size);
	}
	return 0.0f;
}

ZBRUSH_API float GetChannel(char* pStrValue, double optValue, char* pOptBuffer1, int optBuffer1Size, char* pOptBuffer2, int optBuffer2Size, char** zData)
{
	SurfaceMap::iterator i = surfaceMap.find((int)optValue);
	if (i != surfaceMap.end())
	{
		i->second->getChannel(mapChannel(pStrValue), pOptBuffer1, optBuffer1Size);
	}
	return 0.0f;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // attach to process
            // return FALSE to fail DLL load
            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // succesful
}
