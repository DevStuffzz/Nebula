#pragma once

#include "Nebula/Audio/AudioClip.h"
#include <AL/al.h>
#include <string>

namespace Nebula {

	class OpenALAudioClip : public AudioClip
	{
	public:
		OpenALAudioClip(const std::string& filepath);
		virtual ~OpenALAudioClip();

		virtual float GetDuration() const override { return m_Duration; }
		virtual int GetSampleRate() const override { return m_SampleRate; }
		virtual int GetChannels() const override { return m_Channels; }
		virtual const std::string& GetFilePath() const override { return m_FilePath; }

		virtual void* GetNativeHandle() const override { return (void*)(uintptr_t)m_BufferID; }
		ALuint GetBufferID() const { return m_BufferID; }

	private:
		bool LoadWAV(const std::string& filepath);
		bool LoadMP3(const std::string& filepath);
		bool LoadOGG(const std::string& filepath);

	private:
		std::string m_FilePath;
		ALuint m_BufferID = 0;
		float m_Duration = 0.0f;
		int m_SampleRate = 0;
		int m_Channels = 0;
	};

}
