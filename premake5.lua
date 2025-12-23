workspace "Nebula"
    architecture "x64"
  
    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    startproject "Runtime"



IncludeDir = {}
IncludeDir["GLFW"] = "Nebula/vendor/GLFW/include"
IncludeDir["glad"] = "Nebula/vendor/glad/include"
IncludeDir["ImGui"] = "Nebula/vendor/imgui"
IncludeDir["ImGuizmo"] = "Nebula/vendor/ImGuizmo"
IncludeDir["glm"] = "Nebula/vendor/glm"
IncludeDir["stb_image"] = "Nebula/vendor/stb_image"
IncludeDir["entt"] = "Nebula/vendor/entt/src"
IncludeDir["json"] = "Nebula/vendor/json"
IncludeDir["spdlog"] = "Nebula/vendor/spdlog"
IncludeDir["bullet3"] = "Nebula/vendor/bullet3/src"
IncludeDir["OpenAL"] = "Nebula/vendor/openal-soft/include"
IncludeDir["dr_libs"] = "Nebula/vendor"
IncludeDir["mono"] = "Nebula/vendor/mono-build/include/mono-2.0"


group "Dependencies"
include "Nebula/vendor/GLFW"
include "Nebula/vendor/imgui"
include "Nebula/vendor/ImGuizmo"
include "Nebula/vendor/glad"
group "Dependencies/Bullet"
include "Nebula/vendor/bullet3"
group ""

-- C# Script Projects
include "NebulaScriptCore/premake5.lua"
include "Scripts/premake5.lua"

project "Nebula"
    location "Nebula"

    kind "SharedLib"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "nbpch.h"
    pchsource "Nebula/src/nbpch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    removefiles
    {
        "%{prj.name}/src/Platform/Windows/**",
        "%{prj.name}/src/Platform/MacOS/**"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.ImGuizmo}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.json}",
        "%{IncludeDir.bullet3}",
        "%{IncludeDir.OpenAL}",
        "%{IncludeDir.dr_libs}",
        "%{IncludeDir.mono}",
    }

    links
    {
        "GLFW",
        "ImGui",
        "ImGuizmo",
        "glad",
        "Bullet3",
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "Off"
        systemversion "latest"
        buildoptions { "/utf-8", "/FS" }

        files
        {
            "%{prj.name}/src/Platform/Windows/**.h",
            "%{prj.name}/src/Platform/Windows/**.cpp"
        }

        removefiles
        {
            "%{prj.name}/src/Platform/OpenAL/**"
        }

        files
        {
            "%{prj.name}/src/Platform/OpenAL/**.h",
            "%{prj.name}/src/Platform/OpenAL/**.cpp"
        }

        defines
        {
            "NB_PLATFORM_WINDOWS",
            "NB_BUILD_DLL",
            "NOMINMAX"
        }

        links
        {
            "opengl32.lib"
        }

        libdirs
        {
            "Nebula/vendor/openal-soft/build/%{cfg.buildcfg}",
            "Nebula/vendor/mono-build/lib"
        }

        links
        {
            "OpenAL32.lib",
            "mono-2.0-sgen.lib"
        }

        postbuildcommands
        {
            "{COPYFILE} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Runtime/\"",
            "{COPYFILE} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Cosmic/\"",
            "{COPYDIR} ../assets ../bin/" .. outputdir .. "/Nebula/assets",
            "{COPYFILE} ../Nebula/vendor/openal-soft/build/%{cfg.buildcfg}/OpenAL32.dll \"../bin/" .. outputdir .. "/Nebula/\"",
            "{COPYFILE} ../Nebula/vendor/openal-soft/build/%{cfg.buildcfg}/OpenAL32.dll \"../bin/" .. outputdir .. "/Runtime/\"",
            "{COPYFILE} ../Nebula/vendor/openal-soft/build/%{cfg.buildcfg}/OpenAL32.dll \"../bin/" .. outputdir .. "/Cosmic/\"",
            "{COPYFILE} ../Nebula/vendor/mono-build/mono-2.0-sgen.dll \"../bin/" .. outputdir .. "/Runtime/\"",
            "{COPYFILE} ../Nebula/vendor/mono-build/mono-2.0-sgen.dll \"../bin/" .. outputdir .. "/Cosmic/\"",
            "{COPYDIR} ../Nebula/vendor/mono-build/lib/4.5 ../bin/" .. outputdir .. "/Runtime/lib/mono/4.5",
            "{COPYDIR} ../Nebula/vendor/mono-build/lib/4.5 ../bin/" .. outputdir .. "/Cosmic/lib/mono/4.5",
        }

        linkoptions { "/ignore:4099" } -- NOTE(Peter): Disable no PDB found warning
        disablewarnings { "4068" } -- Disable "Unknown #pragma mark warning"


    filter "system:macosx"
        cppdialect "C++17"
        systemversion "10.15"

        files
        {
            "%{prj.name}/src/Platform/MacOS/**.h",
            "%{prj.name}/src/Platform/MacOS/**.cpp",
            "%{prj.name}/src/Platform/MacOS/**.mm"
        }

        defines
        {
            "NB_PLATFORM_MACOS",
            "NB_BUILD_DLL",
            "GL_SILENCE_DEPRECATION"
        }

        links
        {
            "OpenGL.framework",
            "Cocoa.framework",
            "IOKit.framework",
            "CoreVideo.framework"
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/")
        }

    
    filter "configurations:Debug"
        defines "NB_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "NB_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "NB_DIST"
        optimize "On"

project "Runtime"
    location "Runtime"
    kind "ConsoleApp"
    language "C++"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    
    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }
    
    includedirs
    {
        "Nebula/src",
        "%{prj.name}/src",
        "Nebula/vendor/glm",
        "%{IncludeDir.entt}",
        "%{IncludeDir.spdlog}",
    }
    
    links
    {
        "Nebula",
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "Off"
        systemversion "latest"
        buildoptions { "/utf-8", "/FS" }

        defines
        {
            "NB_PLATFORM_WINDOWS",
        }

        debugdir ("bin/" .. outputdir .. "/Runtime")

        postbuildcommands
        {
            "{COPYDIR} ../bin/" .. outputdir .. "/Cosmic/assets ../bin/" .. outputdir .. "/Runtime/assets"
        }



    filter "system:macosx"
        cppdialect "C++17"
        systemversion "10.15"

        defines
        {
            "NB_PLATFORM_MACOS",
            "GL_SILENCE_DEPRECATION"
        }
    
    filter "configurations:Debug"
        defines "NB_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "NB_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "NB_DIST"
        optimize "On"

project "Cosmic"
    location "Cosmic"
    kind "ConsoleApp"
    language "C++"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    
    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }
    
    includedirs
    {
        "Nebula/src",
        "%{prj.name}/src",
        "Nebula/vendor/glm",
        "%{IncludeDir.entt}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.spdlog}",
    }
    
    links
    {
        "Nebula",
        "ImGui",
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "Off"
        systemversion "latest"
        buildoptions { "/utf-8", "/FS" }

        defines
        {
            "NB_PLATFORM_WINDOWS",
            "NOMINMAX"
        }

        debugdir ("bin/" .. outputdir .. "/Cosmic")

    filter "system:macosx"
        cppdialect "C++17"
        systemversion "10.15"

        defines
        {
            "NB_PLATFORM_MACOS"
        }
    
    filter "configurations:Debug"
        defines "NB_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "NB_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "NB_DIST"
        optimize "On"
