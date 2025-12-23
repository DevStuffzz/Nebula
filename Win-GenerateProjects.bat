
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

REM ============================================
REM Check for Mono binaries
REM ============================================
set MONO_BUILD_DIR=Nebula\vendor\mono-build
set MONO_DLL=%MONO_BUILD_DIR%\mono-2.0-sgen.dll
set MONO_LIB=%MONO_BUILD_DIR%\lib\mono-2.0-sgen.lib
set MONO_INCLUDE=%MONO_BUILD_DIR%\include\mono-2.0\mono\jit\jit.h
set MONO_URL=https://download.mono-project.com/archive/6.12.0/windows-installer/mono-6.12.0.206-x64-0.msi

if exist "%MONO_DLL%" if exist "%MONO_LIB%" if exist "%MONO_INCLUDE%" (
    echo Found Mono binaries in vendor directory
    goto :generate_projects
)

echo Mono binaries not found in vendor directory
echo.
echo Please install Mono to enable C# scripting:
echo 1. Download from: https://www.mono-project.com/download/stable/
echo 2. Install to default location (C:\Program Files\Mono)
echo 3. Run: setup-mono.ps1 to copy files
echo.
echo Or continue without Mono (C# scripting disabled)
echo.
timeout /t 3
goto :generate_projects

:generate_projects
REM Generate Visual Studio 2022 projects
call "%PREMAKE_PATH%" vs2022

echo.
echo ============================================
echo Visual Studio projects generated successfully!
echo Open Nebula.sln to start building the project.
echo ============================================
pause