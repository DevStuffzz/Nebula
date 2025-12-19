#pragma once
#pragma warning(disable: 4251)

#include "Nebula/Core.h"
#include <string>
#include <memory>

namespace Nebula {

	// Abstract audio clip interface
	class NEBULA_API AudioClip
	{
	public:
		virtual ~AudioClip() = default;

		// Load audio from file
		static AudioClip* Create(const std::string& filepath);

		// Get clip properties
		virtual float GetDuration() const = 0;
		virtual int GetSampleRate() const = 0;
		virtual int GetChannels() const = 0;
		virtual const std::string& GetFilePath() const = 0;

		// Runtime data access (platform-specific)
		virtual void* GetNativeHandle() const = 0;
	};

}
