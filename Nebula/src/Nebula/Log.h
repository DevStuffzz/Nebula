#pragma once
#pragma warning(disable: 4251)

#include "nbpch.h"

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"


namespace Nebula {

	
	enum LogLevel {
		LOG_TRACE, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL
	};

	struct LogMessage {
		std::string Message;
		LogLevel Level = LogLevel::LOG_TRACE;

		LogMessage(const std::string& message) : Message(message) {
		}

		LogMessage(const std::string& message, const LogLevel level) : Message(message), Level(level) {
		}
	};

	class NEBULA_API Log{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_CosmicLogger; }
		inline static std::shared_ptr<std::vector<LogMessage>>& GetClientMessages() { return s_Messages; }
		inline static std::shared_ptr<spdlog::logger>& GetCosmicLogger() { return s_ClientLogger; }


		inline static void LogClientMessage(const std::string& message) {
			LogClientMessage(message, LogLevel::LOG_TRACE);
		}

		inline static void LogClientMessage(const std::string& message, const LogLevel level) {
			s_Messages->push_back({message, level});
		}

		inline static void ClearClientMessages() {
			s_Messages->clear();
		}

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_CosmicLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;

		static std::shared_ptr<std::vector<LogMessage>> s_Messages;
	};
}

#define NB_CORE_ERROR(...)  ::Nebula::Log::GetCoreLogger()->error(__VA_ARGS__)
#define NB_CORE_WARN(...)   ::Nebula::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define NB_CORE_INFO(...)   ::Nebula::Log::GetCoreLogger()->info(__VA_ARGS__)
#define NB_CORE_TRACE(...)  ::Nebula::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define NB_CORE_FATAL(...)  ::Nebula::Log::GetCoreLogger()->fatal(__VA_ARGS__)

#define NB_ERROR(...)		::Nebula::Log::GetClientLogger()->error(__VA_ARGS__)
#define NB_WARN(...)		::Nebula::Log::GetClientLogger()->warn(__VA_ARGS__)
#define NB_INFO(...)		::Nebula::Log::GetClientLogger()->info(__VA_ARGS__)
#define NB_TRACE(...)		::Nebula::Log::GetClientLogger()->trace(__VA_ARGS__)
#define NB_fatal(...)		::Nebula::Log::GetClientLogger()->fatal(__VA_ARGS__)

#define COSMIC_ERROR(...)	::Nebula::Log::GetCosmicLogger()->error(__VA_ARGS__)
#define COSMIC_WARN(...)	::Nebula::Log::GetCosmicLogger()->warn(__VA_ARGS__)
#define COSMIC_INFO(...)	::Nebula::Log::GetCosmicLogger()->info(__VA_ARGS__)
#define COSMIC_TRACE(...)	::Nebula::Log::GetCosmicLogger()->trace(__VA_ARGS__)
#define COSMIC_fatal(...)	::Nebula::Log::GetCosmicLogger()->fatal(__VA_ARGS__)