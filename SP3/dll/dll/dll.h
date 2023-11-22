#pragma once
#include <minwindef.h>

#ifdef MYLIBRARY_EXPORTS
#define MYLIBRARY_API __declspec(dllexport)
#else
#define MYLIBRARY_API __declspec(dllimport)
#endif

struct Params
{
	char left[255];
	char right[255];
};

extern "C" MYLIBRARY_API void ReplaceStr(DWORD processId, const char* strForSearch, const char* strReplace);
extern "C" MYLIBRARY_API void ReplaceStrInject(Params * params);