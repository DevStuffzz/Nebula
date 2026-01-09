#!/usr/bin/env pwsh
# Script to automatically commit and push all git submodules

param(
    [string]$CommitMessage = "Update submodule",
    [switch]$DryRun = $false
)

Write-Host "=== Git Submodule Auto Commit & Push ===" -ForegroundColor Cyan
Write-Host ""

# Check if we're in a git repository
if (-not (Test-Path ".git")) {
    Write-Host "Error: Not a git repository" -ForegroundColor Red
    exit 1
}

# Get all submodules
$submodules = git config --file .gitmodules --get-regexp path | ForEach-Object {
    $_ -replace '^submodule\.(.+)\.path ', ''
}

if ($submodules.Count -eq 0) {
    Write-Host "No submodules found in this repository" -ForegroundColor Yellow
    exit 0
}

Write-Host "Found $($submodules.Count) submodule(s)" -ForegroundColor Green
Write-Host ""

$successCount = 0
$skippedCount = 0
$errorCount = 0

foreach ($submodule in $submodules) {
    Write-Host "Processing: $submodule" -ForegroundColor Cyan
    
    if (-not (Test-Path $submodule)) {
        Write-Host "  [SKIP] Path does not exist" -ForegroundColor Yellow
        $skippedCount++
        continue
    }
    
    Push-Location $submodule
    
    try {
        # Check if there are any changes
        $status = git status --porcelain
        
        if ([string]::IsNullOrWhiteSpace($status)) {
            Write-Host "  [SKIP] No changes to commit" -ForegroundColor Yellow
            $skippedCount++
        }
        else {
            Write-Host "  Changes detected:" -ForegroundColor Yellow
            git status --short
            
            if ($DryRun) {
                Write-Host "  [DRY-RUN] Would commit and push changes" -ForegroundColor Magenta
                $successCount++
            }
            else {
                # Stage all changes
                git add -A
                
                # Commit
                git commit -m $CommitMessage
                if ($LASTEXITCODE -ne 0) {
                    Write-Host "  [ERROR] Failed to commit" -ForegroundColor Red
                    $errorCount++
                }
                else {
                    # Push
                    git push
                    if ($LASTEXITCODE -ne 0) {
                        Write-Host "  [ERROR] Failed to push" -ForegroundColor Red
                        $errorCount++
                    }
                    else {
                        Write-Host "  [SUCCESS] Committed and pushed" -ForegroundColor Green
                        $successCount++
                    }
                }
            }
        }
    }
    catch {
        Write-Host "  [ERROR] $_" -ForegroundColor Red
        $errorCount++
    }
    finally {
        Pop-Location
    }
    
    Write-Host ""
}

# Summary
Write-Host "=== Summary ===" -ForegroundColor Cyan
Write-Host "Success: $successCount" -ForegroundColor Green
Write-Host "Skipped: $skippedCount" -ForegroundColor Yellow
Write-Host "Errors:  $errorCount" -ForegroundColor Red

if ($DryRun) {
    Write-Host ""
    Write-Host "This was a dry run. Use without -DryRun to actually commit and push." -ForegroundColor Magenta
}

exit $errorCount
