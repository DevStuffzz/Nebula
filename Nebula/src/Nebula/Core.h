#pragma once
#pragma warning(disable: 4251)


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

// Standard type aliases for easier engine development
#include <cstdint>

namespace Nebula {
    // Standard sized integer types
    using uint8  = std::uint8_t;
    using uint16 = std::uint16_t; 
    using uint32 = std::uint32_t;
    using uint64 = std::uint64_t;
    
    using int8  = std::int8_t;
    using int16 = std::int16_t;
    using int32 = std::int32_t;
    using int64 = std::int64_t;
    
    // Common aliases
    using uint = unsigned int;
    
    // Floating point types
    using float32 = float;
    using float64 = double;
}