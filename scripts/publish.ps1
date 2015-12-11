#########################################################
# Script to build and package OpenRCT2
# - Sets the source code preprocessor defines
# - Builds a clean release of OpenRCT2
# - Creates a ZIP for distribution
#########################################################
param (
    [string]$server  = "",
    [string]$buildNo = ""
)

# Get paths
$scriptPath   = $Script:MyInvocation.MyCommand.Path
$scriptsPath  = Split-Path $scriptPath
$rootPath     = Split-Path $scriptsPath

# Set build attributes
function do-prepareSource($build_server = "", $build_number = "")
{
    Write-Host "Setting build #defines..." -ForegroundColor Cyan
    # $build_number = "";
    # $build_server = "";
    $build_branch = (git rev-parse --abbrev-ref HEAD)
    $build_commit_sha1 = (git rev-parse HEAD)
    $build_commit_sha1_short = (git rev-parse --short HEAD)

    $defines = @{ }
    $defines["OPENRCT2_BUILD_NUMBER"]      = $build_number;
    $defines["OPENRCT2_BUILD_SERVER"]      = $build_server;
    $defines["OPENRCT2_BRANCH"]            = $build_branch;
    $defines["OPENRCT2_COMMIT_SHA1"]       = $build_commit_sha1;
    $defines["OPENRCT2_COMMIT_SHA1_SHORT"] = $build_commit_sha1_short;

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

    # Create archive
    7za a -tzip -mx9 $outZip "$tempDir\*"

    # Remove temp directory
    Remove-Item -Force -Recurse $tempDir -ErrorAction SilentlyContinue
}

do-prepareSource $server $buildNo
do-build
do-package
