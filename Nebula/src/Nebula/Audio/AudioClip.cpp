#include "nbpch.h"
#include "AudioClip.h"

#ifdef NB_PLATFORM_WINDOWS
	#include "Platform/OpenAL/OpenALAudioClip.h"
#endif

namespace Nebula {

	AudioClip* AudioClip::Create(const std::string& filepath)
	{
	#ifdef NB_PLATFORM_WINDOWS
		return new OpenALAudioClip(filepath);
	#else
		NB_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
	#endif
	}

}
