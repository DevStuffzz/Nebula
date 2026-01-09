@echo off
setlocal enabledelayedexpansion

echo ============================================
echo Nebula ScriptCore Build Script
echo ============================================
echo.

REM Check for MSBuild
set "MSBUILD_PATH="
for %%v in (2022 2019) do (
    for %%e in (Enterprise Professional Community BuildTools) do (
        if exist "C:\Program Files\Microsoft Visual Studio\%%v\%%e\MSBuild\Current\Bin\MSBuild.exe" (
            set "MSBUILD_PATH=C:\Program Files\Microsoft Visual Studio\%%v\%%e\MSBuild\Current\Bin\MSBuild.exe"
            goto :found_msbuild
        )
    )
)

REM Try to find MSBuild via vswhere
if exist "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" (
    for /f "usebackq tokens=*" %%i in (`"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.Component.MSBuild -property installationPath`) do (
        set "VS_PATH=%%i"
    )
    if exist "!VS_PATH!\MSBuild\Current\Bin\MSBuild.exe" (
        set "MSBUILD_PATH=!VS_PATH!\MSBuild\Current\Bin\MSBuild.exe"
        goto :found_msbuild
    )
)

echo ERROR: MSBuild not found!
echo Please install Visual Studio 2019 or 2022 with .NET desktop development workload.
pause
exit /b 1

:found_msbuild
echo Found MSBuild: %MSBUILD_PATH%
echo.

REM Build Debug configuration
echo Building NebulaScriptCore (Debug)...
echo ----------------------------------------
"%MSBUILD_PATH%" "NebulaScriptCore\NebulaScriptCore\NebulaScriptCore.csproj" /p:Configuration=Debug /p:Platform=x64 /t:Build /v:minimal
if %errorlevel% neq 0 (
    echo.
    echo ERROR: Debug build failed!
    exit /b %errorlevel%
)

echo.
echo NebulaScriptCore Debug build completed successfully!
echo.

REM Build Release configuration
echo Building NebulaScriptCore (Release)...
echo ----------------------------------------
"%MSBUILD_PATH%" "NebulaScriptCore\NebulaScriptCore\NebulaScriptCore.csproj" /p:Configuration=Release /p:Platform=x64 /t:Build /v:minimal
if %errorlevel% neq 0 (
    echo.
    echo ERROR: Release build failed!
    exit /b %errorlevel%
)

echo.
echo ============================================
echo NebulaScriptCore built successfully!
echo ============================================
echo.
echo Output locations:
echo   Debug:   bin\Debug-windows-x86_64\NebulaScriptCore\
echo   Release: bin\Release-windows-x86_64\NebulaScriptCore\
