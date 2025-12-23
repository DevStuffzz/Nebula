#include "nbpch.h"
#include "ScriptBuilder.h"
#include "Nebula/Log.h"
#include "Nebula/Project/Project.h"
#include <fstream>
#include <sstream>

namespace Nebula {

	bool ScriptBuilder::BuildProjectScripts(const std::filesystem::path& projectPath)
	{
		std::filesystem::path scriptDir = projectPath / "Scripts";
		std::filesystem::path csprojPath = scriptDir / "Scripts.csproj";

		// Generate .csproj if it doesn't exist or needs update
		if (!std::filesystem::exists(csprojPath))
		{
			if (!GenerateProjectFile(projectPath))
				return false;
		}

		// Run MSBuild
		return RunMSBuild(csprojPath);
	}

	bool ScriptBuilder::GenerateProjectFile(const std::filesystem::path& projectPath)
	{
		std::filesystem::path scriptDir = projectPath / "Scripts";
		std::filesystem::create_directories(scriptDir);
		
		std::filesystem::path csprojPath = scriptDir / "Scripts.csproj";

		// Get the NebulaScriptCore.dll path (relative to the project)
		std::filesystem::path enginePath = std::filesystem::current_path();
		std::filesystem::path coreAssemblyPath = enginePath / "bin" / "Debug-windows-x86_64" / "NebulaScriptCore" / "NebulaScriptCore.dll";

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

	std::vector<std::string> ScriptBuilder::GetCSharpFiles(const std::filesystem::path& scriptDir)
	{
		std::vector<std::string> files;
		
		if (!std::filesystem::exists(scriptDir))
			return files;

		for (const auto& entry : std::filesystem::recursive_directory_iterator(scriptDir))
		{
			if (entry.is_regular_file() && entry.path().extension() == ".cs")
			{
				files.push_back(entry.path().string());
			}
		}

		return files;
	}

	bool ScriptBuilder::RunMSBuild(const std::filesystem::path& csprojPath)
	{
		// Build the command
		std::string command = "dotnet build \"" + csprojPath.string() + "\" -c Debug --nologo -v q";

		NB_CORE_INFO("Building project scripts...");
		NB_CORE_TRACE("Command: {0}", command);

		int result = std::system(command.c_str());
		
		if (result == 0)
		{
			NB_CORE_INFO("Scripts built successfully!");
			return true;
		}
		else
		{
			NB_CORE_ERROR("Failed to build scripts (exit code: {0})", result);
			return false;
		}
	}

}
