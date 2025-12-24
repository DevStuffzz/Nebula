@echo off
setlocal

echo Building Nebula Engine...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64

echo Building Nebula core projects (incremental)...
MSBuild.exe Nebula\Nebula.vcxproj /p:Configuration=Debug /p:Platform=x64 /v:minimal /m
if %ERRORLEVEL% NEQ 0 (
    echo Nebula.dll build failed!
    pause
    exit /b %ERRORLEVEL%
)

MSBuild.exe Cosmic\Cosmic.vcxproj /p:Configuration=Debug /p:Platform=x64 /v:minimal /m
if %ERRORLEVEL% NEQ 0 (
    echo Cosmic.exe build failed!
    pause
    exit /b %ERRORLEVEL%
)

MSBuild.exe CosmicLauncher\CosmicLauncher.vcxproj /p:Configuration=Debug /p:Platform=x64 /v:minimal /m
if %ERRORLEVEL% NEQ 0 (
    echo CosmicLauncher.exe build failed!
    pause
    exit /b %ERRORLEVEL%
)

echo Build successful! Copying binaries...
copy /Y bin\Debug-windows-x86_64\Cosmic\Cosmic.exe bin\Debug-windows-x86_64\CosmicLauncher\Cosmic\
copy /Y bin\Debug-windows-x86_64\Nebula\Nebula.dll bin\Debug-windows-x86_64\CosmicLauncher\Cosmic\

echo Starting Cosmic Launcher...
cd bin\Debug-windows-x86_64\CosmicLauncher
CosmicLauncher.exe