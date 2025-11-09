#pragma once


#ifdef NB_PLATFORM_WINDOWS
#ifdef NB_BUILD_DLL
#define NEBULA_API __declspec(dllexport)
#else
#define NEBULA_API __declspec(dllimport)
#endif
#else
#error Nebula only supports Windows!
#endif