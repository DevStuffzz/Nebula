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

#define BIT(x) (1 << x)


#ifdef NEB_ENABLE_ASSERTS
#define NEB_ASSERT(x, ...) { if(!(x)) { NB_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define NEB_CORE_ASSERT(x, ...) { if(!(x)) { NB_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define NEB_ASSERT(x, ...)
#define NEB_CORE_ASSERT(x, ...)
#endif

#include "Nebula/Log.h"