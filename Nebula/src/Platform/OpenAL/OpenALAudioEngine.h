#pragma once

#include "Nebula/Audio/AudioEngine.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <unordered_map>

namespace Nebula {

	class OpenALAudioEngine : public AudioEngine
	{
	public:
		OpenALAudioEngine();
		virtual ~OpenALAudioEngine();

		virtual bool Init() override;
		virtual void Shutdown() override;

		virtual void SetMasterVolume(float volume) override;
		virtual float GetMasterVolume() const override { return m_MasterVolume; }

		virtual void SetListenerPosition(const glm::vec3& position) override;
		virtual void SetListenerVelocity(const glm::vec3& velocity) override;
		virtual void SetListenerOrientation(const glm::vec3& forward, const glm::vec3& up) override;

		virtual uint32_t CreateSource() override;
		virtual void DestroySource(uint32_t sourceID) override;

		virtual void PlaySource(uint32_t sourceID) override;
		virtual void PauseSource(uint32_t sourceID) override;
		virtual void StopSource(uint32_t sourceID) override;
		virtual void StopAll() override;
		virtual bool IsSourcePlaying(uint32_t sourceID) const override;

		virtual void SetSourceClip(uint32_t sourceID, AudioClip* clip) override;
		virtual void SetSourcePosition(uint32_t sourceID, const glm::vec3& position) override;
		virtual void SetSourceVelocity(uint32_t sourceID, const glm::vec3& velocity) override;
		virtual void SetSourceVolume(uint32_t sourceID, float volume) override;
		virtual void SetSourcePitch(uint32_t sourceID, float pitch) override;
		virtual void SetSourceLoop(uint32_t sourceID, bool loop) override;
		virtual void SetSourceSpatial(uint32_t sourceID, bool spatial) override;

		virtual float GetSourceVolume(uint32_t sourceID) const override;
		virtual float GetSourcePitch(uint32_t sourceID) const override;
		virtual bool GetSourceLoop(uint32_t sourceID) const override;

		virtual void SetSourceRolloffFactor(uint32_t sourceID, float rolloff) override;
		virtual void SetSourceReferenceDistance(uint32_t sourceID, float distance) override;
		virtual void SetSourceMaxDistance(uint32_t sourceID, float distance) override;

		virtual void Update() override;

	private:
		ALuint GetALSource(uint32_t sourceID) const;

	private:
		ALCdevice* m_Device = nullptr;
		ALCcontext* m_Context = nullptr;
		float m_MasterVolume = 1.0f;

		std::unordered_map<uint32_t, ALuint> m_Sources;
		uint32_t m_NextSourceID = 1;
	};

}
