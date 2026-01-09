#include "nbpch.h"
#include "OpenALAudioEngine.h"
#include "OpenALAudioClip.h"
#include "Nebula/Log.h"

namespace Nebula {

	OpenALAudioEngine::OpenALAudioEngine()
	{
	}

	OpenALAudioEngine::~OpenALAudioEngine()
	{
		Shutdown();
	}

	bool OpenALAudioEngine::Init()
	{
		NB_CORE_INFO("Initializing OpenAL Audio Engine...");

		// Open default audio device
		m_Device = alcOpenDevice(nullptr);
		if (!m_Device)
		{
			NB_CORE_ERROR("Failed to open OpenAL device");
			return false;
		}

		// Create audio context
		m_Context = alcCreateContext(m_Device, nullptr);
		if (!m_Context)
		{
			NB_CORE_ERROR("Failed to create OpenAL context");
			alcCloseDevice(m_Device);
			m_Device = nullptr;
			return false;
		}

		// Make context current
		if (!alcMakeContextCurrent(m_Context))
		{
			NB_CORE_ERROR("Failed to make OpenAL context current");
			alcDestroyContext(m_Context);
			alcCloseDevice(m_Device);
			m_Context = nullptr;
			m_Device = nullptr;
			return false;
		}

		// Set default listener properties
		alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
		alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
		ALfloat listenerOri[] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f }; // Forward and Up vectors
		alListenerfv(AL_ORIENTATION, listenerOri);
		alListenerf(AL_GAIN, m_MasterVolume);

		// Check for errors
		ALenum error = alGetError();
		if (error != AL_NO_ERROR)
		{
			NB_CORE_ERROR("OpenAL initialization error: {0}", error);
			return false;
		}

		// Log OpenAL version info
		const ALCchar* deviceName = alcGetString(m_Device, ALC_DEVICE_SPECIFIER);
		NB_CORE_INFO("OpenAL Device: {0}", deviceName ? deviceName : "Unknown");
		NB_CORE_INFO("OpenAL Vendor: {0}", alGetString(AL_VENDOR));
		NB_CORE_INFO("OpenAL Renderer: {0}", alGetString(AL_RENDERER));
		NB_CORE_INFO("OpenAL Version: {0}", alGetString(AL_VERSION));

		NB_CORE_INFO("OpenAL Audio Engine initialized successfully");
		return true;
	}

	void OpenALAudioEngine::Shutdown()
	{
		// Delete all sources
		for (auto& pair : m_Sources)
		{
			alDeleteSources(1, &pair.second);
		}
		m_Sources.clear();

		// Cleanup OpenAL
		if (m_Context)
		{
			alcMakeContextCurrent(nullptr);
			alcDestroyContext(m_Context);
			m_Context = nullptr;
		}

		if (m_Device)
		{
			alcCloseDevice(m_Device);
			m_Device = nullptr;
		}

		NB_CORE_INFO("OpenAL Audio Engine shut down");
	}

	void OpenALAudioEngine::SetMasterVolume(float volume)
	{
		m_MasterVolume = glm::clamp(volume, 0.0f, 1.0f);
		alListenerf(AL_GAIN, m_MasterVolume);
	}

	void OpenALAudioEngine::SetListenerPosition(const glm::vec3& position)
	{
		alListener3f(AL_POSITION, position.x, position.y, position.z);
	}

	void OpenALAudioEngine::SetListenerVelocity(const glm::vec3& velocity)
	{
		alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
	}

	void OpenALAudioEngine::SetListenerOrientation(const glm::vec3& forward, const glm::vec3& up)
	{
		ALfloat orientation[] = { forward.x, forward.y, forward.z, up.x, up.y, up.z };
		alListenerfv(AL_ORIENTATION, orientation);
	}

	uint32_t OpenALAudioEngine::CreateSource()
	{
		ALuint source;
		alGenSources(1, &source);

		ALenum error = alGetError();
		if (error != AL_NO_ERROR)
		{
			NB_CORE_ERROR("Failed to create OpenAL source: {0}", error);
			return 0;
		}

		// Set default source properties
		alSourcef(source, AL_PITCH, 1.0f);
		alSourcef(source, AL_GAIN, 1.0f);
		alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
		alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
		alSourcei(source, AL_LOOPING, AL_FALSE);
		alSourcei(source, AL_SOURCE_RELATIVE, AL_FALSE); // Spatial audio by default

		// Set spatial audio parameters
		alSourcef(source, AL_ROLLOFF_FACTOR, 1.0f);
		alSourcef(source, AL_REFERENCE_DISTANCE, 1.0f);
		alSourcef(source, AL_MAX_DISTANCE, 100.0f);

		uint32_t sourceID = m_NextSourceID++;
		m_Sources[sourceID] = source;

		return sourceID;
	}

	void OpenALAudioEngine::DestroySource(uint32_t sourceID)
	{
		auto it = m_Sources.find(sourceID);
		if (it != m_Sources.end())
		{
			alDeleteSources(1, &it->second);
			m_Sources.erase(it);
		}
	}

	void OpenALAudioEngine::PlaySource(uint32_t sourceID)
	{
		ALuint source = GetALSource(sourceID);
		if (source != 0)
		{
			alSourcePlay(source);
		}
	}

	void OpenALAudioEngine::PauseSource(uint32_t sourceID)
	{
		ALuint source = GetALSource(sourceID);
		if (source != 0)
		{
			alSourcePause(source);
		}
	}

	void OpenALAudioEngine::StopSource(uint32_t sourceID)
	{
		ALuint source = GetALSource(sourceID);
		if (source != 0)
		{
			alSourceStop(source);
		}
	}

	void OpenALAudioEngine::StopAll()
	{
		for (const auto& pair : m_Sources)
		{
			ALuint source = pair.second;
			alSourceStop(source);
			// Rewind to beginning to fully reset the source
			alSourceRewind(source);
		}
	}

	bool OpenALAudioEngine::IsSourcePlaying(uint32_t sourceID) const
	{
		ALuint source = GetALSource(sourceID);
		if (source != 0)
		{
			ALint state;
			alGetSourcei(source, AL_SOURCE_STATE, &state);
			return state == AL_PLAYING;
		}
		return false;
	}

	void OpenALAudioEngine::SetSourceClip(uint32_t sourceID, AudioClip* clip)
	{
		ALuint source = GetALSource(sourceID);
		if (source != 0 && clip != nullptr)
		{
			OpenALAudioClip* openalClip = static_cast<OpenALAudioClip*>(clip);
			alSourcei(source, AL_BUFFER, openalClip->GetBufferID());
		}
	}

	void OpenALAudioEngine::SetSourcePosition(uint32_t sourceID, const glm::vec3& position)
	{
		ALuint source = GetALSource(sourceID);
		if (source != 0)
		{
			alSource3f(source, AL_POSITION, position.x, position.y, position.z);
		}
	}

	void OpenALAudioEngine::SetSourceVelocity(uint32_t sourceID, const glm::vec3& velocity)
	{
		ALuint source = GetALSource(sourceID);
		if (source != 0)
		{
			alSource3f(source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
		}
	}

	void OpenALAudioEngine::SetSourceVolume(uint32_t sourceID, float volume)
	{
		ALuint source = GetALSource(sourceID);
		if (source != 0)
		{
			alSourcef(source, AL_GAIN, glm::clamp(volume, 0.0f, 1.0f));
		}
	}

	void OpenALAudioEngine::SetSourcePitch(uint32_t sourceID, float pitch)
	{
		ALuint source = GetALSource(sourceID);
		if (source != 0)
		{
			alSourcef(source, AL_PITCH, glm::max(pitch, 0.01f));
		}
	}

	void OpenALAudioEngine::SetSourceLoop(uint32_t sourceID, bool loop)
	{
		ALuint source = GetALSource(sourceID);
		if (source != 0)
		{
			alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
		}
	}

	void OpenALAudioEngine::SetSourceSpatial(uint32_t sourceID, bool spatial)
	{
		ALuint source = GetALSource(sourceID);
		if (source != 0)
		{
			// AL_SOURCE_RELATIVE: true = non-spatial (relative to listener), false = spatial (world position)
			alSourcei(source, AL_SOURCE_RELATIVE, spatial ? AL_FALSE : AL_TRUE);
		}
	}

	float OpenALAudioEngine::GetSourceVolume(uint32_t sourceID) const
	{
		ALuint source = GetALSource(sourceID);
		if (source != 0)
		{
			ALfloat volume;
			alGetSourcef(source, AL_GAIN, &volume);
			return volume;
		}
		return 0.0f;
	}

	float OpenALAudioEngine::GetSourcePitch(uint32_t sourceID) const
	{
		ALuint source = GetALSource(sourceID);
		if (source != 0)
		{
			ALfloat pitch;
			alGetSourcef(source, AL_PITCH, &pitch);
			return pitch;
		}
		return 1.0f;
	}

	bool OpenALAudioEngine::GetSourceLoop(uint32_t sourceID) const
	{
		ALuint source = GetALSource(sourceID);
		if (source != 0)
		{
			ALint loop;
			alGetSourcei(source, AL_LOOPING, &loop);
			return loop == AL_TRUE;
		}
		return false;
	}

	void OpenALAudioEngine::SetSourceRolloffFactor(uint32_t sourceID, float rolloff)
	{
		ALuint source = GetALSource(sourceID);
		if (source != 0)
		{
			alSourcef(source, AL_ROLLOFF_FACTOR, glm::max(rolloff, 0.0f));
		}
	}

	void OpenALAudioEngine::SetSourceReferenceDistance(uint32_t sourceID, float distance)
	{
		ALuint source = GetALSource(sourceID);
		if (source != 0)
		{
			alSourcef(source, AL_REFERENCE_DISTANCE, glm::max(distance, 0.0f));
		}
	}

	void OpenALAudioEngine::SetSourceMaxDistance(uint32_t sourceID, float distance)
	{
		ALuint source = GetALSource(sourceID);
		if (source != 0)
		{
			alSourcef(source, AL_MAX_DISTANCE, glm::max(distance, 0.0f));
		}
	}

	void OpenALAudioEngine::Update()
	{
		// OpenAL handles most updates internally
		// This can be used for streaming audio or other per-frame updates if needed
	}

	ALuint OpenALAudioEngine::GetALSource(uint32_t sourceID) const
	{
		auto it = m_Sources.find(sourceID);
		if (it != m_Sources.end())
		{
			return it->second;
		}
		return 0;
	}

}
