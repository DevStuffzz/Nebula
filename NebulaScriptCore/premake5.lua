project "NebulaScriptCore"
    location "NebulaScriptCore"
    kind "SharedLib"
    language "C#"
    dotnetframework "netstandard2.1"
    csversion "9.0"
    
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
    
    files
    {
        "Source/**.cs"
    }
    
    filter "configurations:Debug"
        optimize "Off"
        symbols "On"
        
    filter "configurations:Release"
        optimize "On"
        symbols "On"
        
    filter "configurations:Dist"
        optimize "Full"
        symbols "Off"
