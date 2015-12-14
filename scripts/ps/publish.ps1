#########################################################
# Script to build and package OpenRCT2
# - Sets the source code preprocessor defines
# - Builds a clean release of OpenRCT2
# - Creates a ZIP for distribution
#########################################################
param (
    [string]$Server  = "",
    [string]$BuildNumber = "",
    [string]$GitBranch = "",
    [switch]$Installer = $false
)

# Setup
$ErrorActionPreference = "Stop"
$scriptsPath = Split-Path $Script:MyInvocation.MyCommand.Path
Import-Module "$scriptsPath\common.psm1" -DisableNameChecking

# Get paths
$rootPath = Get-RootPath

# Set build attributes
function do-prepareSource()
{
    Write-Host "Setting build #defines..." -ForegroundColor Cyan
    if ($GitBranch -eq "")
    {
        $GitBranch = (git rev-parse --abbrev-ref HEAD)
    }
    $GitCommitSha1 = (git rev-parse HEAD)
    $GitCommitSha1Short = (git rev-parse --short HEAD)

    $defines = @{ }
    $defines["OPENRCT2_BUILD_NUMBER"]      = $BuildNumber;
    $defines["OPENRCT2_BUILD_SERVER"]      = $Server;
    $defines["OPENRCT2_BRANCH"]            = $GitBranch;
    $defines["OPENRCT2_COMMIT_SHA1"]       = $GitCommitSha1;
    $defines["OPENRCT2_COMMIT_SHA1_SHORT"] = $GitCommitSha1Short;

    $defineString = ""
    foreach ($key in $defines.Keys) {
        $value = $defines[$key]
        if ($value -is [System.String]) {
            $value = $value.Replace('"', '\"')
        }
        $defineString += "$key=""$value"";";
    }

    # Set the environment variable which the msbuild project will use
    $env:OPENRCT2_DEFINES = $defineString;
}

# Building OpenRCT2
function do-build()
{
    Write-Host "Building OpenRCT2..." -ForegroundColor Cyan
    & "$scriptsPath\build.ps1" all -Rebuild
}

# Package
function do-package()
{
    Write-Host "Publishing OpenRCT2..." -ForegroundColor Cyan
    $releaseDir = "$rootPath\bin"
    $distDir    = "$rootPath\distribution"
    $tempDir    = "$rootPath\artifacts\temp"
    $outZip     = "$rootPath\artifacts\openrct2.zip"

    # Create new temp directory
    Remove-Item -Force -Recurse $tempDir -ErrorAction SilentlyContinue
    New-Item -Force -ItemType Directory $tempDir > $null

    # Copy files to be archived
    Copy-Item -Force -Recurse "$releaseDir\data"               $tempDir -ErrorAction Stop
    Copy-Item -Force          "$releaseDir\openrct2.exe"       $tempDir -ErrorAction Stop
    Copy-Item -Force          "$releaseDir\openrct2.dll"       $tempDir -ErrorAction Stop
    Copy-Item -Force          "$releaseDir\curl-ca-bundle.crt" $tempDir -ErrorAction Stop
    Copy-Item -Force          "$releaseDir\SDL2.dll"           $tempDir -ErrorAction Stop
    Copy-Item -Force          "$distDir\changelog.txt"         $tempDir -ErrorAction Stop
    Copy-Item -Force          "$distDir\known_issues.txt"      $tempDir -ErrorAction Stop
    Copy-Item -Force          "$distDir\readme.txt"            $tempDir -ErrorAction Stop

    # Create archive using 7z (renowned for speed and compression)
    $7zcmd = "7za"
    if (-not (AppExists($7zcmd)))
    {
        # AppVeyor in particular uses '7z' instead
        $7zcmd = "7z"
        if (-not (AppExists($7zcmd)))
        {
            Write-Host "Publish script requires 7z to be in PATH" -ForegroundColor Red
            exit 1
        }
    }
    & $7zcmd a -tzip -mx9 $outZip "$tempDir\*"

    # Remove temp directory
    Remove-Item -Force -Recurse $tempDir -ErrorAction SilentlyContinue
}

# Installer
function do-installer()
{
    Write-Host "Publishing OpenRCT2..." -ForegroundColor Cyan
    $artifactsDir = "$rootPath\artifacts"
    $installerDir = "$rootPath\distribution\windows"

    # Create artifacts directory
    New-Item -Force -ItemType Directory $artifactsDir > $null

    # Create installer
    & "$installerDir\build.ps1"
    if ($LASTEXITCODE -ne 0)
    {
        Write-Host "Failed to create installer." -ForegroundColor Red
        exit 1
    }

    $binaries = (Get-ChildItem "$installerDir\*.exe" | Sort-Object -Property LastWriteTime -Descending)
    if ($binaries -eq 0)
    {
        Write-Host "Unable to find created installer." -ForegroundColor Red
        exit 1
    }

    Copy-Item $binaries[0].FullName $artifactsDir
}

do-prepareSource
do-build
if ($Installer)
{
    do-installer
}
else
{
    do-package
}
