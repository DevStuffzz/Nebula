#include "nbpch.h"
#include "OpenALAudioClip.h"
#include "Nebula/Log.h"
#include <AL/al.h>
#include <fstream>
#include <cstring>

#define DR_MP3_IMPLEMENTATION
#include "dr_libs/dr_mp3.h"

namespace Nebula {

	// WAV file header structure
	struct WAVHeader
	{
		char RIFF[4];        // "RIFF"
		uint32_t ChunkSize;
		char WAVE[4];        // "WAVE"
		char fmt[4];         // "fmt "
		uint32_t Subchunk1Size;
		uint16_t AudioFormat;
		uint16_t NumChannels;
		uint32_t SampleRate;
		uint32_t ByteRate;
		uint16_t BlockAlign;
		uint16_t BitsPerSample;
		char data[4];        // "data"
		uint32_t Subchunk2Size;
	};

	OpenALAudioClip::OpenALAudioClip(const std::string& filepath)
		: m_FilePath(filepath)
	{
		// Generate OpenAL buffer
		alGenBuffers(1, &m_BufferID);
		
		// Check file extension and load appropriately
		if (filepath.find(".wav") != std::string::npos || filepath.find(".WAV") != std::string::npos)
		{
			if (!LoadWAV(filepath))
			{
				NB_CORE_ERROR("Failed to load WAV audio file: {0}", filepath);
			}
		}
		else if (filepath.find(".mp3") != std::string::npos || filepath.find(".MP3") != std::string::npos)
		{
			if (!LoadMP3(filepath))
			{
				NB_CORE_ERROR("Failed to load MP3 audio file: {0}", filepath);
			}
		}
		else if (filepath.find(".ogg") != std::string::npos || filepath.find(".OGG") != std::string::npos)
		{
			if (!LoadOGG(filepath))
			{
				NB_CORE_ERROR("Failed to load OGG audio file: {0}", filepath);
			}
		}
		else
		{
			NB_CORE_ERROR("Unsupported audio format: {0}", filepath);
		}
	}

	OpenALAudioClip::~OpenALAudioClip()
	{
		if (m_BufferID != 0)
		{
			alDeleteBuffers(1, &m_BufferID);
		}
	}

	bool OpenALAudioClip::LoadWAV(const std::string& filepath)
	{
		std::ifstream file(filepath, std::ios::binary);
		if (!file.is_open())
		{
			NB_CORE_ERROR("Failed to open WAV file: {0}", filepath);
			return false;
		}

		// Read RIFF header
		char riff[4];
		uint32_t chunkSize;
		char wave[4];
		file.read(riff, 4);
		file.read(reinterpret_cast<char*>(&chunkSize), 4);
		file.read(wave, 4);

		if (std::strncmp(riff, "RIFF", 4) != 0 || std::strncmp(wave, "WAVE", 4) != 0)
		{
			NB_CORE_ERROR("Invalid WAV file format: {0}", filepath);
			return false;
		}

		// Read fmt chunk
		char fmt[4];
		uint32_t fmtSize;
		uint16_t audioFormat;
		uint16_t numChannels;
		uint32_t sampleRate;
		uint32_t byteRate;
		uint16_t blockAlign;
		uint16_t bitsPerSample;

		file.read(fmt, 4);
		file.read(reinterpret_cast<char*>(&fmtSize), 4);
		file.read(reinterpret_cast<char*>(&audioFormat), 2);
		file.read(reinterpret_cast<char*>(&numChannels), 2);
		file.read(reinterpret_cast<char*>(&sampleRate), 4);
		file.read(reinterpret_cast<char*>(&byteRate), 4);
		file.read(reinterpret_cast<char*>(&blockAlign), 2);
		file.read(reinterpret_cast<char*>(&bitsPerSample), 2);

		// Skip extra format bytes if present
		if (fmtSize > 16)
		{
			file.seekg(fmtSize - 16, std::ios::cur);
		}

		// Find data chunk
		char dataHeader[4];
		uint32_t dataSize;
		while (file.read(dataHeader, 4))
		{
			file.read(reinterpret_cast<char*>(&dataSize), 4);
			if (std::strncmp(dataHeader, "data", 4) == 0)
				break;
			// Skip this chunk
			file.seekg(dataSize, std::ios::cur);
		}

		// Store properties
		m_SampleRate = sampleRate;
		m_Channels = numChannels;
		m_Duration = static_cast<float>(dataSize) / static_cast<float>(byteRate);

		// Read audio data
		std::vector<char> audioData(dataSize);
		file.read(audioData.data(), dataSize);
		file.close();

		// Determine OpenAL format
		ALenum format = 0;
		std::vector<int16_t> convertedData;
		const char* dataToUpload = audioData.data();
		size_t uploadSize = audioData.size();

		if (numChannels == 1 && bitsPerSample == 8)
			format = AL_FORMAT_MONO8;
		else if (numChannels == 1 && bitsPerSample == 16)
			format = AL_FORMAT_MONO16;
		else if (numChannels == 2 && bitsPerSample == 8)
			format = AL_FORMAT_STEREO8;
		else if (numChannels == 2 && bitsPerSample == 16)
			format = AL_FORMAT_STEREO16;
		else if (bitsPerSample == 32 && audioFormat == 3) // IEEE Float
		{
			// Convert 32-bit float to 16-bit PCM
			NB_CORE_WARN("Converting 32-bit float audio to 16-bit PCM: {0}", filepath);
			
			size_t sampleCount = audioData.size() / sizeof(float);
			convertedData.resize(sampleCount);
			
			const float* floatData = reinterpret_cast<const float*>(audioData.data());
			for (size_t i = 0; i < sampleCount; ++i)
			{
				// Clamp and convert float [-1.0, 1.0] to int16
				float sample = floatData[i];
				sample = (sample < -1.0f) ? -1.0f : (sample > 1.0f) ? 1.0f : sample;
				convertedData[i] = static_cast<int16_t>(sample * 32767.0f);
			}
			
			dataToUpload = reinterpret_cast<const char*>(convertedData.data());
			uploadSize = convertedData.size() * sizeof(int16_t);
			format = (numChannels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
		}
		else
		{
			NB_CORE_ERROR("Unsupported WAV format: {0} channels, {1} bits, format {2}", numChannels, bitsPerSample, audioFormat);
			return false;
		}

		// Upload to OpenAL buffer
		alBufferData(m_BufferID, format, dataToUpload, static_cast<ALsizei>(uploadSize), sampleRate);

		// Check for errors
		ALenum error = alGetError();
		if (error != AL_NO_ERROR)
		{
			NB_CORE_ERROR("OpenAL error loading WAV: {0}", error);
			return false;
		}

		NB_CORE_INFO("Loaded WAV audio: {0} ({1} channels, {2} Hz, {3:.2f}s)", filepath, m_Channels, m_SampleRate, m_Duration);
		return true;
	}

	bool OpenALAudioClip::LoadMP3(const std::string& filepath)
	{
		drmp3_config config;
		drmp3_uint64 totalFrameCount;
		drmp3_int16* pSampleData = drmp3_open_file_and_read_pcm_frames_s16(filepath.c_str(), &config, &totalFrameCount, nullptr);
		
		if (pSampleData == nullptr)
		{
			NB_CORE_ERROR("Failed to decode MP3 file: {0}", filepath);
			return false;
		}

		// Store properties
		m_SampleRate = config.sampleRate;
		m_Channels = config.channels;
		m_Duration = static_cast<float>(totalFrameCount) / static_cast<float>(config.sampleRate);

		// Determine OpenAL format
		ALenum format = (config.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
		
		// Calculate data size in bytes
		size_t dataSize = totalFrameCount * config.channels * sizeof(drmp3_int16);

		// Upload to OpenAL buffer
		alBufferData(m_BufferID, format, pSampleData, static_cast<ALsizei>(dataSize), config.sampleRate);

		// Free the decoded data
		drmp3_free(pSampleData, nullptr);

		// Check for errors
		ALenum error = alGetError();
		if (error != AL_NO_ERROR)
		{
			NB_CORE_ERROR("OpenAL error loading MP3: {0}", error);
			return false;
		}

		NB_CORE_INFO("Loaded MP3 audio: {0} ({1} channels, {2} Hz, {3:.2f}s)", filepath, m_Channels, m_SampleRate, m_Duration);
		return true;
	}

	bool OpenALAudioClip::LoadOGG(const std::string& filepath)
	{
		// TODO: Implement OGG loading using stb_vorbis or libvorbis
		NB_CORE_WARN("OGG loading not yet implemented for: {0}", filepath);
		return false;
	}

}
