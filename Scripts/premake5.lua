project "Scripts"
    location "Scripts"
    kind "SharedLib"
    language "C#"
    dotnetframework "net8.0"
    
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
    
    files
    {
        "**.cs"
    }
    
    links
    {
        "NebulaScriptCore"
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
