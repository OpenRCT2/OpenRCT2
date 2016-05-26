#########################################################
# Script to build different parts of OpenRCT2
#########################################################
[CmdletBinding()]
param (
    [Parameter(Mandatory = $false, Position = 1)]
    [string]$Task = "openrct2",

    [Parameter(Mandatory = $false)]
    [string]$Configuration = "Release",

    [Parameter(Mandatory = $false)]
    [switch]$Rebuild = $false,

    [Parameter(Mandatory = $false)]
    [switch]$Breakpad = $false
)

# Setup
$ErrorActionPreference = "Stop"
$scriptsPath = Split-Path $Script:MyInvocation.MyCommand.Path
Import-Module "$scriptsPath\common.psm1" -DisableNameChecking

# Get paths
$rootPath     = Get-RootPath
$binPath      = Join-Path $rootPath "bin"
$openrct2Path = Join-Path $binPath "openrct2.exe"

function Build-Data()
{
    $dataPath    = "$rootPath\data"
    $binDataPath = "$binPath\data"

    # Create data directory in bin
    Write-Host "Create data directory..." -ForegroundColor Cyan
    New-Item -Force -ItemType Directory $binDataPath > $null

    # Create symlinks
    Symlink-or-Copy "$binDataPath\language" "$dataPath\language" > $null
    Symlink-or-Copy "$binDataPath\title"    "$dataPath\title"    > $null

    return 0
}

function Build-OpenRCT2()
{
    Write-Host "Building OpenRCT2 ($Configuration)..." -ForegroundColor Cyan
    
    $target = ""
    if ($Rebuild)
    {
        $target = "/t:rebuild"
    }
    msbuild $rootPath\openrct2.sln /p:Breakpad=$Breakpad /p:Configuration=$Configuration /p:Platform=Win32 $target /v:minimal | Write-Host
    return $LASTEXITCODE
}

function Build-G2()
{
    # Check if OpenRCT2 binary exists
    if (-not (Test-Path $openrct2Path))
    {
        Write-Host "You must build OpenRCT2 first before g2.dat can be built." -ForegroundColor Red
        return 1
    }

    Write-Host "Building g2.dat..." -ForegroundColor Cyan
    & $openrct2Path sprite build "$binPath\data\g2.dat" "$rootPath\resources\g2"
    return $LASTEXITCODE
}

function Build-All()
{
    if (($result = (Build-Data    )) -ne 0) { return $result }
    if (($result = (Build-OpenRCT2)) -ne 0) { return $result }
    if (($result = (Build-G2      )) -ne 0) { return $result }
}

# Script entry point
switch ($Task)
{
    "data"     { $result = Build-Data }
    "openrct2" { $result = Build-OpenRCT2 }
    "g2"       { $result = Build-G2 }
    "all"      { $result = Build-All }
    default    { Write-Host "Unknown build task." -ForegroundColor Red
                 $result = 1 }
}
exit $result
