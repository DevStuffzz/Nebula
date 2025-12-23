@echo off
setlocal

echo Building Cosmic Editor...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64
MSBuild.exe Nebula.sln /p:Configuration=Debug /p:Platform=x64 /v:minimal /m /t:Cosmic

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    pause
    exit /b %ERRORLEVEL%
)

echo Build successful! Running Cosmic...
cd bin\Debug-windows-x86_64\Cosmic
Cosmic.exe
