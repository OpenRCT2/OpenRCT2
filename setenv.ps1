###########################################################
# Setups a PowerShell environment for OpenRCT2 development
###########################################################

function AppExists($app)
{
    $result = (Get-Command $app -CommandType Application -ErrorAction SilentlyContinue)
    return ($result -ne $null -and $result.Count -gt 0)
}

function AddPath($path)
{
    $env:path = "$path;$env:path"
}

$rootPath = Split-Path $Script:MyInvocation.MyCommand.Path
$scriptsPath = "$rootPath\scripts\ps"
Write-Host "Setting up OpenRCT2 development environment for Windows" -ForegroundColor Cyan

$appExists = @{}
$appExists["msbuild"] = AppExists("msbuild");
$appExists["7za"]     = AppExists("7za");
$appExists["7z"]      = AppExists("7z");

if (-not $appExists["msbuild"])
{
    $lookPath = Join-Path ${env:ProgramFiles(x86)} "MSBuild\14.0\Bin\amd64\MSBuild.exe"
    if (Test-Path $lookPath)
    {
        AddPath($lookPath)
    }
    else
    {
        Write-Host "Warning: msbuild not found, please include it in your PATH" -ForegroundColor Yellow
    }
}

AddPath($scriptsPath)
