#ifndef __MAIN_H__
#define __MAIN_H__

#define ZBRUSH_API extern "C" __declspec(dllexport)

//ZBrush expects these arguments for a dll function, though it's best to only use the first three
ZBRUSH_API float Version(char* pDontCare, double optValue, char* pOptBuffer1, int optBuffer1Size, char* pOptBuffer2, int optBuffer2Size, char** zData);
ZBRUSH_API float HelloDLL(char* sometext, double optValue, char* pOptBuffer1, int optBuffer1Size, char* pOptBuffer2, int optBuffer2Size, char** zData);

ZBRUSH_API float CreateSurface(char* sometext, double optValue, char* pOptBuffer1, int optBuffer1Size, char* pOptBuffer2, int optBuffer2Size, char** zData);
ZBRUSH_API float DestroySurface(char* sometext, double optValue, char* pOptBuffer1, int optBuffer1Size, char* pOptBuffer2, int optBuffer2Size, char** zData);
ZBRUSH_API float ReadSurface(char* sometext, double optValue, char* pOptBuffer1, int optBuffer1Size, char* pOptBuffer2, int optBuffer2Size, char** zData);
ZBRUSH_API float WriteSurface(char* sometext, double optValue, char* pOptBuffer1, int optBuffer1Size, char* pOptBuffer2, int optBuffer2Size, char** zData);
ZBRUSH_API float GetWidth(char* sometext, double optValue, char* pOptBuffer1, int optBuffer1Size, char* pOptBuffer2, int optBuffer2Size, char** zData);
ZBRUSH_API float GetHeight(char* sometext, double optValue, char* pOptBuffer1, int optBuffer1Size, char* pOptBuffer2, int optBuffer2Size, char** zData);
ZBRUSH_API float SetChannel(char* sometext, double optValue, char* pOptBuffer1, int optBuffer1Size, char* pOptBuffer2, int optBuffer2Size, char** zData);
ZBRUSH_API float GetChannel(char* sometext, double optValue, char* pOptBuffer1, int optBuffer1Size, char* pOptBuffer2, int optBuffer2Size, char** zData);

#endif // __MAIN_H__