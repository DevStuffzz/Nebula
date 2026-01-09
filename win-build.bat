@echo off
cls
setlocal enabledelayedexpansion

set DUMP_FILE=bin\win-build-dump.txt
echo Build started at %date% %time% > %DUMP_FILE%
echo. >> %DUMP_FILE%

echo ========================================
echo Nebula Engine Build Script
echo ========================================
echo.

REM Stop all background build services FIRST
echo Stopping .NET build servers...
dotnet build-server shutdown >nul 2>&1

echo Terminating all related processes...
taskkill /F /IM CosmicLauncher.exe >nul 2>&1
taskkill /F /IM Cosmic.exe >nul 2>&1
taskkill /F /IM Runtime.exe >nul 2>&1
taskkill /F /IM devenv.exe >nul 2>&1
taskkill /F /IM MSBuild.exe >nul 2>&1
taskkill /F /IM VBCSCompiler.exe >nul 2>&1
taskkill /F /IM cl.exe >nul 2>&1
taskkill /F /IM link.exe >nul 2>&1
taskkill /F /IM csc.exe >nul 2>&1
taskkill /F /IM dotnet.exe >nul 2>&1

echo Waiting for processes to fully terminate...
timeout /t 5 /nobreak >nul

echo.
echo Setting up build environment...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul 2>&1

echo.
echo ========================================
echo Building NebulaScriptCore (C#)
echo ========================================
pushd NebulaScriptCore\NebulaScriptCore
dotnet build NebulaScriptCore.csproj -c Debug --nologo -v quiet
set SCRIPTCORE_ERROR=%ERRORLEVEL%
popd

if !SCRIPTCORE_ERROR! NEQ 0 (
    echo ERROR: NebulaScriptCore build failed!
    exit /b !SCRIPTCORE_ERROR!
)
echo NebulaScriptCore built successfully.

REM Wait for file handles
timeout /t 2 /nobreak >nul

echo.
echo ========================================
echo Building C++ Projects
echo ========================================
echo Building Nebula.dll...
MSBuild.exe Nebula\Nebula.vcxproj /p:Configuration=Debug /p:Platform=x64 /v:quiet /nologo /maxcpucount >> %DUMP_FILE% 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Nebula.dll build failed! Check %DUMP_FILE%
    exit /b %ERRORLEVEL%
)

echo Building Cosmic.exe...
MSBuild.exe Cosmic\Cosmic.vcxproj /p:Configuration=Debug /p:Platform=x64 /v:quiet /nologo /maxcpucount >> %DUMP_FILE% 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Cosmic.exe build failed! Check %DUMP_FILE%
    exit /b %ERRORLEVEL%
)

echo Building CosmicLauncher.exe...
MSBuild.exe CosmicLauncher\CosmicLauncher.vcxproj /p:Configuration=Debug /p:Platform=x64 /v:quiet /nologo /maxcpucount >> %DUMP_FILE% 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CosmicLauncher.exe build failed! Check %DUMP_FILE%
    exit /b %ERRORLEVEL%
)

echo.
echo ========================================
echo Build Complete!
echo ========================================

REM Verify outputs exist
if not exist "bin\Debug-windows-x86_64\CosmicLauncher\CosmicLauncher.exe" (
    echo ERROR: CosmicLauncher.exe was not created! Build may have failed silently.
    echo Check %DUMP_FILE% for details.
    exit /b 1
)

echo.
echo Build completed at %date% %time% >> %DUMP_FILE%
echo Build log saved to %DUMP_FILE%
echo.
echo Launching Cosmic Launcher...
start "" "bin\Debug-windows-x86_64\CosmicLauncher\CosmicLauncher.exe"