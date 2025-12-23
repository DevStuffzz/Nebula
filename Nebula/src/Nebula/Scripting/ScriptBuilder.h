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
		
		// Generate the .csproj file for the project scripts
		static bool GenerateProjectFile(const std::filesystem::path& projectPath);

	private:
		static std::vector<std::string> GetCSharpFiles(const std::filesystem::path& scriptDir);
		static bool RunMSBuild(const std::filesystem::path& csprojPath);
	};

}
