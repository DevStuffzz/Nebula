
@echo off

set PREMAKE_PATH=vendor\bin\premake\premake5.exe
set PREMAKE_DIR=vendor\bin\premake
set PREMAKE_URL=https://github.com/premake/premake-core/releases/download/v5.0.0-beta2/premake-5.0.0-beta2-windows.zip

REM Check if premake5.exe exists in vendor path
if exist "%PREMAKE_PATH%" (
    echo Found premake5.exe in vendor directory
    goto :generate
)

echo premake5.exe not found in vendor directory
echo Downloading premake5.exe...

REM Create vendor directory structure if it doesn't exist
if not exist "%PREMAKE_DIR%" mkdir "%PREMAKE_DIR%"

REM Download premake5 using PowerShell
powershell -Command "& {[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri '%PREMAKE_URL%' -OutFile 'premake-temp.zip'}"

if not exist "premake-temp.zip" (
    echo Error: Failed to download premake5
    echo Please download premake5 manually from: https://premake.github.io/
    pause
    exit /b 1
)

echo Extracting premake5.exe...
powershell -Command "& {Expand-Archive -Path 'premake-temp.zip' -DestinationPath '%PREMAKE_DIR%' -Force}"
del premake-temp.zip

if not exist "%PREMAKE_PATH%" (
    echo Error: Failed to extract premake5.exe
    pause
    exit /b 1
)

echo premake5.exe downloaded successfully!

:generate
REM Generate Visual Studio 2022 projects
call "%PREMAKE_PATH%" vs2022

echo Visual Studio projects generated successfully!
echo Open Nebula.sln to start building the project.
pause