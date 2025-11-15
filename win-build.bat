@echo off
setlocal

echo Building Nebula...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64
MSBuild.exe Nebula.sln /p:Configuration=Debug /p:Platform=x64 /v:minimal /t:Sandbox

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    pause
    exit /b %ERRORLEVEL%
)

echo Build successful! Running Sandbox...
cd bin\Debug-windows-x86_64\Sandbox
Sandbox.exe
