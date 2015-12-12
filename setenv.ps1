###########################################################
# Setups a PowerShell environment for OpenRCT2 development
###########################################################

# Setup
$ErrorActionPreference = "Stop"
$rootPath = Split-Path $Script:MyInvocation.MyCommand.Path
$scriptsPath = "$rootPath\scripts\ps"
Import-Module "$scriptsPath\common.psm1" -DisableNameChecking

Write-Host "Setting up OpenRCT2 development environment for Windows" -ForegroundColor Cyan

$appExists = @{}
$appExists["msbuild"] = AppExists("msbuild");
$appExists["7za"]     = AppExists("7za");
$appExists["7z"]      = AppExists("7z");

if (-not $appExists["msbuild"])
{
    if ($env:PROCESSOR_ARCHITECTURE -eq "AMD64")
    {
        $lookPath = Join-Path ${env:ProgramFiles(x86)} "MSBuild\14.0\Bin\amd64\MSBuild.exe"
    }
    else
    {
        $lookPath = Join-Path $env:ProgramFiles "MSBuild\14.0\Bin\MSBuild.exe"
    }
    if (Test-Path $lookPath)
    {
        AddPath(Split-Path $lookPath)
    }
    else
    {
        Write-Host "Warning: msbuild not found, please include it in your PATH" -ForegroundColor Yellow
    }
}

AddPath($scriptsPath)
