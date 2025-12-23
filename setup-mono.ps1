# Mono Setup Helper Script
# Run this if Win-GenerateProjects.bat couldn't download Mono automatically

Write-Host "====================================" -ForegroundColor Cyan
Write-Host "Mono Setup Helper" -ForegroundColor Cyan
Write-Host "====================================" -ForegroundColor Cyan
Write-Host ""

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$monoBuildDir = Join-Path $scriptDir "Nebula\vendor\mono-build"

# Check if Mono is already installed system-wide
$monoPaths = @(
    "C:\Program Files\Mono",
    "C:\Program Files (x86)\Mono",
    "$env:ProgramFiles\Mono",
    "${env:ProgramFiles(x86)}\Mono"
)

$monoPath = $null
foreach ($path in $monoPaths) {
    if (Test-Path $path) {
        $monoPath = $path
        Write-Host "Found Mono installation at: $monoPath" -ForegroundColor Green
        break
    }
}

if ($monoPath) {
    Write-Host ""
    Write-Host "Copying Mono files from system installation..." -ForegroundColor Yellow
    
    # Create necessary directories
    New-Item -ItemType Directory -Path "$monoBuildDir\include" -Force | Out-Null
    New-Item -ItemType Directory -Path "$monoBuildDir\lib" -Force | Out-Null
    
    # Copy headers (only mono-2.0 subdirectory to avoid nested include/include structure)
    if (Test-Path "$monoPath\include\mono-2.0") {
        Copy-Item "$monoPath\include\mono-2.0" -Destination "$monoBuildDir\include\" -Recurse -Force
        Write-Host "  Copied include files" -ForegroundColor Green
    } else {
        Write-Host "  Warning: mono-2.0 headers not found" -ForegroundColor Yellow
    }
    
    # Copy only the required library file
    if (Test-Path "$monoPath\lib\mono-2.0-sgen.lib") {
        Copy-Item "$monoPath\lib\mono-2.0-sgen.lib" -Destination "$monoBuildDir\lib\" -Force
        Write-Host "  Copied library file" -ForegroundColor Green
    } else {
        Write-Host "  Warning: mono-2.0-sgen.lib not found" -ForegroundColor Yellow
    }
    
    # Copy DLL
    if (Test-Path "$monoPath\bin\mono-2.0-sgen.dll") {
        Copy-Item "$monoPath\bin\mono-2.0-sgen.dll" -Destination "$monoBuildDir\" -Force
        Write-Host "  Copied mono-2.0-sgen.dll" -ForegroundColor Green
    } else {
        Write-Host "  Warning: mono-2.0-sgen.dll not found" -ForegroundColor Yellow
    }
    
    # Copy BCL assemblies (mscorlib.dll, System.dll, etc.)
    if (Test-Path "$monoPath\lib\mono") {
        Copy-Item "$monoPath\lib\mono\*" -Destination "$monoBuildDir\lib\" -Recurse -Force
        Write-Host "  Copied BCL assemblies" -ForegroundColor Green
    } else {
        Write-Host "  Warning: Mono BCL assemblies not found" -ForegroundColor Yellow
    }
    
    Write-Host ""
    Write-Host "Mono setup complete!" -ForegroundColor Green
    Write-Host "Run Win-GenerateProjects.bat to continue" -ForegroundColor White
} else {
    Write-Host "Mono not found on system." -ForegroundColor Red
    Write-Host ""
    Write-Host "Please choose an option:" -ForegroundColor Yellow
    Write-Host "1. Download and install Mono from: https://www.mono-project.com/download/stable/" -ForegroundColor White
    Write-Host "   Then run this script again" -ForegroundColor White
    Write-Host ""
    Write-Host "2. Or download pre-built binaries manually:" -ForegroundColor White
    Write-Host "   https://github.com/winsoft666/Mono/releases/download/v6.12.0.122/mono-6.12.0.122-x64-bin.zip" -ForegroundColor White
    Write-Host "   Extract to: $monoBuildDir" -ForegroundColor White
}

Write-Host ""
Write-Host "====================================" -ForegroundColor Cyan
pause
