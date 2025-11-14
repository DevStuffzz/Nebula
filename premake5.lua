workspace "Nebula"
    architecture "x64"
  
    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    startproject "Sandbox"



IncludeDir = {}
IncludeDir["GLFW"] = "Nebula/vendor/GLFW/include"
IncludeDir["ImGui"] = "Nebula/vendor/imgui"
IncludeDir["glm"] = "Nebula/vendor/glm"

include "Nebula/vendor/GLFW"
include "Nebula/vendor/imgui"

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

    includedirs
    {
        "%{prj.name}/vendor/spdlog/include",
        "%{prj.name}/src",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
    }

    links
    {
        "GLFW",
        "ImGui"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "Off"
        systemversion "latest"
        buildoptions { "/utf-8" }

        defines
        {
            "NB_PLATFORM_WINDOWS",
            "NB_BUILD_DLL"
        }

        links
        {
            "opengl32.lib"
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox\"")
        }

    filter "system:macosx"
        cppdialect "C++17"
        systemversion "10.15"

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

project "Sandbox"
    location "Sandbox"
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
        "Nebula/vendor/spdlog/include",
        "Nebula/src",
        "%{prj.name}/src",
    }
    
    links
    {
        "Nebula"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "Off"
        systemversion "latest"
        buildoptions { "/utf-8" }

        defines
        {
            "NB_PLATFORM_WINDOWS",
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
