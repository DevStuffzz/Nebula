#include "nbpch.h"
#include "ScriptBuilder.h"
#include "ScriptEngine.h"
#include "Nebula/Log.h"
#include "Nebula/Project/Project.h"
#include <fstream>
#include <sstream>
#include <windows.h>

namespace Nebula {

	static std::string s_LastBuildError = "";

	bool ScriptBuilder::BuildProjectScripts(const std::filesystem::path& projectPath)
	{
		std::filesystem::path scriptDir = projectPath / "Assets" / "Scripts";
		std::filesystem::path csprojPath = scriptDir / "Scripts.csproj";

		// Check if .csproj needs regeneration
		bool needsRegeneration = false;
		if (!std::filesystem::exists(csprojPath))
		{
			needsRegeneration = true;
		}
		else
		{
			// Check if the .csproj has the old incorrect path format
			std::ifstream file(csprojPath);
			if (file.is_open())
			{
				std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
				file.close();
				
				// If it contains an absolute path or doesn't have the relative path, regenerate it
				if (content.find("D:\\dev\\Nebula\\bin") != std::string::npos || 
				    content.find("..\\..\\Library\\NebulaScriptCore.dll") == std::string::npos)
				{
					needsRegeneration = true;
					NB_CORE_WARN("Detected old .csproj format, regenerating...");
				}
			}
		}

		if (needsRegeneration)
		{
			if (!GenerateProjectFile(projectPath))
				return false;
		}

		// Run MSBuild
		return RunMSBuild(csprojPath);
	}

	bool ScriptBuilder::GenerateProjectFile(const std::filesystem::path& projectPath)
	{
		std::filesystem::path scriptDir = projectPath / "Assets" / "Scripts";
		std::filesystem::create_directories(scriptDir);
		
		std::filesystem::path csprojPath = scriptDir / "Scripts.csproj";

		// Use relative path to NebulaScriptCore.dll in the project's Library folder
		// From Assets/Scripts/ to Library/NebulaScriptCore.dll
		std::filesystem::path coreAssemblyPath = std::filesystem::path("..") / ".." / "Library" / "NebulaScriptCore.dll";

		std::ofstream file(csprojPath);
		if (!file.is_open())
		{
			NB_CORE_ERROR("Failed to create .csproj file at: {0}", csprojPath.string());
			return false;
		}

		file << R"(<Project Sdk="Microsoft.NET.Sdk">

  <PropertyGroup>
    <TargetFramework>net8.0</TargetFramework>
    <Nullable>disable</Nullable>
    <AllowUnsafeBlocks>true</AllowUnsafeBlocks>
    <LangVersion>latest</LangVersion>
    <OutputPath>bin\$(Configuration)\</OutputPath>
    <AppendTargetFrameworkToOutputPath>false</AppendTargetFrameworkToOutputPath>
  </PropertyGroup>

  <ItemGroup>
    <Reference Include="NebulaScriptCore">
      <HintPath>)" << coreAssemblyPath.string() << R"(</HintPath>
      <Private>false</Private>
    </Reference>
  </ItemGroup>

</Project>
)";

		file.close();
		NB_CORE_INFO("Generated .csproj file for project scripts");
		return true;
	}

	bool ScriptBuilder::RunMSBuild(const std::filesystem::path& csprojPath)
	{
		// Build the command - capture output for error reporting
		std::string command = "dotnet build \"" + csprojPath.string() + "\" -c Debug --nologo 2>&1";

		NB_CORE_INFO("Building project scripts...");
		NB_CORE_TRACE("Command: {0}", command);

		// Use popen to capture output
		FILE* pipe = _popen(command.c_str(), "r");
		if (!pipe)
		{
			s_LastBuildError = "Failed to execute build command";
			NB_CORE_ERROR(s_LastBuildError);
			return false;
		}

		// Read output
		char buffer[256];
		std::stringstream output;
		while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
		{
			output << buffer;
		}

		int result = _pclose(pipe);
		
		if (result == 0)
		{
			NB_CORE_INFO("Scripts built successfully!");
			s_LastBuildError = "";
			return true;
		}
		else
		{
			s_LastBuildError = output.str();
			NB_CORE_ERROR("Failed to build scripts (exit code: {0})", result);
			NB_CORE_ERROR("Build output: {0}", s_LastBuildError);
			return false;
		}
	}

	bool ScriptBuilder::RebuildScripts(const std::filesystem::path& projectPath)
	{
		std::filesystem::path scriptDir = projectPath / "Assets" / "Scripts";
		std::filesystem::path csprojPath = scriptDir / "Scripts.csproj";

		if (!std::filesystem::exists(csprojPath))
		{
			s_LastBuildError = "Scripts.csproj not found";
			return false;
		}

		// Run MSBuild
		if (!RunMSBuild(csprojPath))
			return false;

		// Reload assembly
		std::filesystem::path scriptAssembly = scriptDir / "bin" / "Debug" / "Scripts.dll";
		if (std::filesystem::exists(scriptAssembly))
		{
			ScriptEngine::LoadProjectAssembly(scriptAssembly);
			return true;
		}

		s_LastBuildError = "Scripts.dll not found after build";
		return false;
	}

	std::string ScriptBuilder::GetLastBuildError()
	{
		return s_LastBuildError;
	}

}
