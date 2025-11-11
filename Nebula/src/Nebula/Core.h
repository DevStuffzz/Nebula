#pragma once


#ifdef NB_PLATFORM_WINDOWS
#ifdef NB_BUILD_DLL
#define NEBULA_API __declspec(dllexport)
#else
#define NEBULA_API __declspec(dllimport)
#endif
#elif defined(NB_PLATFORM_MACOS)
#ifdef NB_BUILD_DLL
#define NEBULA_API __attribute__((visibility("default")))
#else
#define NEBULA_API
#endif
#else
#error Nebula only supports Windows and macOS!
#endif

#define BIT(x) (1 << x)

#ifdef NEB_ENABLE_ASSERTS
	#ifdef NB_PLATFORM_WINDOWS
		#define NB_DEBUGBREAK() __debugbreak()
	#elif defined(NB_PLATFORM_MACOS)
		#include <signal.h>
		#define NB_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define NEB_ASSERT(x, ...) { if(!(x)) { NB_ERROR("Assertion Failed: {0}", __VA_ARGS__); NB_DEBUGBREAK(); } }
	#define NEB_CORE_ASSERT(x, ...) { if(!(x)) { NB_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); NB_DEBUGBREAK(); } }
#else
	#define NEB_ASSERT(x, ...)
	#define NEB_CORE_ASSERT(x, ...)
#endif

#include "Nebula/Log.h"