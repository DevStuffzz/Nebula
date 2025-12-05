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
IncludeDir["glm"] = "Nebula/vendor/glm"
IncludeDir["stb_image"] = "Nebula/vendor/stb_image"
IncludeDir["entt"] = "Nebula/vendor/entt/src"
IncludeDir["json"] = "Nebula/vendor/json"
IncludeDir["Lua"] = "Nebula/vendor/lua"

include "Nebula/vendor/GLFW"
include "Nebula/vendor/imgui"
include "Nebula/vendor/glad"
include "Nebula/vendor/lua"

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
        "%{prj.name}/vendor/spdlog/include",
        "%{prj.name}/src",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.json}",
        "%{IncludeDir.Lua}",
    }

    links
    {
        "GLFW",
        "ImGui",
        "glad",
        "Lua",
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "Off"
        systemversion "latest"
        buildoptions { "/utf-8" }

        files
        {
            "%{prj.name}/src/Platform/Windows/**.h",
            "%{prj.name}/src/Platform/Windows/**.cpp"
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

        postbuildcommands
        {
            "{COPYFILE} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Runtime/\"",
            "{COPYFILE} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Cosmic/\"",
            "{COPYDIR} ../assets ../bin/" .. outputdir .. "/Nebula/assets"
        }

    filter "system:macosx"
        cppdialect "C++17"
        systemversion "10.15"

        files
        {
            "%{prj.name}/src/Platform/MacOS/**.h",
            "%{prj.name}/src/Platform/MacOS/**.cpp"
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
        "Nebula/vendor/spdlog/include",
        "Nebula/src",
        "%{prj.name}/src",
        "Nebula/vendor/glm",
        "%{IncludeDir.entt}",
        "%{IncludeDir.Lua}",
    }
    
    links
    {
        "Nebula",
        "Lua",
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
        "Nebula/vendor/spdlog/include",
        "Nebula/src",
        "%{prj.name}/src",
        "Nebula/vendor/glm",
        "%{IncludeDir.entt}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.Lua}",
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
        buildoptions { "/utf-8" }

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
