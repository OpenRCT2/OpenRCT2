#########################################################
# Script to build different parts of OpenRCT2
#########################################################
[CmdletBinding()]
param (
    [Parameter(Mandatory = $false, Position = 1)]
    [string]$Task = "openrct2",

    [Parameter(Mandatory = $false)]
    [string]$Configuration = "Release"
)

# Get paths
$scriptPath   = $Script:MyInvocation.MyCommand.Path
$scriptsPath  = Split-Path $scriptPath
$rootPath     = Split-Path $scriptsPath
$binPath      = Join-Path $rootPath "bin"
$openrct2Path = Join-Path $binPath "openrct2.exe"

function Build-Data()
{
    Write-Host "Copying data to bin..." -ForegroundColor Cyan
    New-Item -Force -ItemType Directory $binPath > $null
    Copy-Item -Force -Recurse "$rootPath\data" $binPath
    return 0
}

function Build-OpenRCT2()
{
    Write-Host "Building OpenRCT2 ($Configuration)..." -ForegroundColor Cyan
    msbuild ..\projects\openrct2.sln /p:Configuration=$Configuration /p:Platform=Win32 /v:minimal | Write-Host
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
