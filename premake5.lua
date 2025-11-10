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

project "Nebula"
    location "Nebula"

    kind "SharedLib"
    language "C++"

    buildoptions { "/utf-8" }       -- <--- add this line here

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
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "NB_PLATFORM_WINDOWS",
            "NB_BUILD_DLL"
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox\"")
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
    
    buildoptions { "/utf-8" }       -- <--- add this line here

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
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "NB_PLATFORM_WINDOWS",
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
