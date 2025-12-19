#include "nbpch.h"
#include "AudioEngine.h"

#ifdef NB_PLATFORM_WINDOWS
	#include "Platform/OpenAL/OpenALAudioEngine.h"
#endif

namespace Nebula {

	AudioEngine* AudioEngine::Create()
	{
	#ifdef NB_PLATFORM_WINDOWS
		return new OpenALAudioEngine();
	#else
		NB_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
	#endif
	}

}
