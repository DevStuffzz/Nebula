#include "nbpch.h"
#include "Log.h"
#include <spdlog/sinks/stdout_color_sinks.h>


namespace Nebula {
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
	std::shared_ptr<spdlog::logger> Log::s_CosmicLogger;

	std::shared_ptr<std::vector<LogMessage>> Log::s_Messages = std::make_shared<std::vector<LogMessage>>();

	void Log::Init() {
		spdlog::set_pattern("%^[%T] %n: %v%$");
		
		s_CoreLogger = spdlog::stdout_color_mt("NEBULA");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("CLIENT");
		s_ClientLogger->set_level(spdlog::level::trace);


		s_CosmicLogger = spdlog::stdout_color_mt("COSMIC");
		s_CosmicLogger->set_level(spdlog::level::trace);
	}
}