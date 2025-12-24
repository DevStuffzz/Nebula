#pragma once

#include "Nebula/Core.h"
#include <string>
#include <filesystem>
#include <vector>

namespace Nebula {

	class NEBULA_API ScriptBuilder
	{
	public:
		// Build the project scripts into an assembly
		static bool BuildProjectScripts(const std::filesystem::path& projectPath);
		
		// Rebuild scripts and reload assembly
		static bool RebuildScripts(const std::filesystem::path& projectPath);
		
		// Generate the .csproj file for the project scripts
		static bool GenerateProjectFile(const std::filesystem::path& projectPath);
		
		// Get last build error message
		static std::string GetLastBuildError();

	private:
		static bool RunMSBuild(const std::filesystem::path& csprojPath);
	};

}
