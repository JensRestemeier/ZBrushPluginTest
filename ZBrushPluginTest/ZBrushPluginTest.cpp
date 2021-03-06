// ZBrushPluginTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>

typedef float(*ZBrushFunc)(char* sometext, double optValue, char* pOptBuffer1, int optBuffer1Size, char* pOptBuffer2, int optBuffer2Size, char** zData);

int main()
{
	// Get Functions
	//HMODULE hm = LoadLibraryEx(L"ZBrushPluginDLL.dll", NULL, 0/*DONT_RESOLVE_DLL_REFERENCES*/);
	HMODULE hm = LoadLibraryEx(L"C:\\Program Files\\Pixologic\\ZBrush 2018\\ZStartup\\ZPlugs64\\ZBrushPluginTest\\ZBrushPluginDLL.dll", NULL, 0/*DONT_RESOLVE_DLL_REFERENCES*/);
	if (hm == NULL) {
		printf("Failed to load DLL 0x%8.8x.", GetLastError());
		return -1;
	}
	ZBrushFunc Version = (ZBrushFunc)GetProcAddress(hm, "Version");
	ZBrushFunc HelloDLL = (ZBrushFunc)GetProcAddress(hm, "HelloDLL");

	ZBrushFunc CreateSurface = (ZBrushFunc)GetProcAddress(hm, "CreateSurface");
	ZBrushFunc DestroySurface = (ZBrushFunc)GetProcAddress(hm, "DestroySurface");
	ZBrushFunc ReadSurface = (ZBrushFunc)GetProcAddress(hm, "ReadSurface");
	ZBrushFunc WriteSurface = (ZBrushFunc)GetProcAddress(hm, "WriteSurface");
	ZBrushFunc GetWidth = (ZBrushFunc)GetProcAddress(hm, "GetWidth");
	ZBrushFunc GetHeight = (ZBrushFunc)GetProcAddress(hm, "GetHeight");
	ZBrushFunc SetChannel = (ZBrushFunc)GetProcAddress(hm, "SetChannel");
	ZBrushFunc GetChannel = (ZBrushFunc)GetProcAddress(hm, "GetChannel");

	// Test functions:
	float version = Version(NULL, 0, NULL, 0, NULL, 0, NULL);
	printf("Version: %f\n", version);

	FreeLibrary(hm);
    return 0;
}

