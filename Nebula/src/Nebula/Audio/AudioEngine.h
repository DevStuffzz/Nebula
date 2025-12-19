#pragma once
#pragma warning(disable: 4251)

#include "Nebula/Core.h"
#include <glm/glm.hpp>
#include <memory>

namespace Nebula {

	class AudioClip;

	// Abstract audio engine interface
	class NEBULA_API AudioEngine
	{
	public:
		virtual ~AudioEngine() = default;

		// Factory method
		static AudioEngine* Create();

		// Engine lifecycle
		virtual bool Init() = 0;
		virtual void Shutdown() = 0;

		// Master volume control (0.0 to 1.0)
		virtual void SetMasterVolume(float volume) = 0;
		virtual float GetMasterVolume() const = 0;

		// Listener (camera/player)
		virtual void SetListenerPosition(const glm::vec3& position) = 0;
		virtual void SetListenerVelocity(const glm::vec3& velocity) = 0;
		virtual void SetListenerOrientation(const glm::vec3& forward, const glm::vec3& up) = 0;

		// Audio source management
		virtual uint32_t CreateSource() = 0;
		virtual void DestroySource(uint32_t sourceID) = 0;

		// Source playback control
		virtual void PlaySource(uint32_t sourceID) = 0;
		virtual void PauseSource(uint32_t sourceID) = 0;
		virtual void StopSource(uint32_t sourceID) = 0;
		virtual void StopAll() = 0;
		virtual bool IsSourcePlaying(uint32_t sourceID) const = 0;

		// Source properties
		virtual void SetSourceClip(uint32_t sourceID, AudioClip* clip) = 0;
		virtual void SetSourcePosition(uint32_t sourceID, const glm::vec3& position) = 0;
		virtual void SetSourceVelocity(uint32_t sourceID, const glm::vec3& velocity) = 0;
		virtual void SetSourceVolume(uint32_t sourceID, float volume) = 0;
		virtual void SetSourcePitch(uint32_t sourceID, float pitch) = 0;
		virtual void SetSourceLoop(uint32_t sourceID, bool loop) = 0;
		virtual void SetSourceSpatial(uint32_t sourceID, bool spatial) = 0;

		// Source property getters
		virtual float GetSourceVolume(uint32_t sourceID) const = 0;
		virtual float GetSourcePitch(uint32_t sourceID) const = 0;
		virtual bool GetSourceLoop(uint32_t sourceID) const = 0;

		// Spatial audio parameters
		virtual void SetSourceRolloffFactor(uint32_t sourceID, float rolloff) = 0;
		virtual void SetSourceReferenceDistance(uint32_t sourceID, float distance) = 0;
		virtual void SetSourceMaxDistance(uint32_t sourceID, float distance) = 0;

		// Update (called per frame if needed)
		virtual void Update() = 0;
	};

}
