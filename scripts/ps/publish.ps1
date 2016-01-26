#########################################################
# Script to build and package OpenRCT2
# - Sets the source code preprocessor defines
# - Builds a clean release of OpenRCT2
# - Creates a ZIP for distribution
#########################################################
param (
    [Parameter(Position = 1)]
    [string]$Task = "all",

    [string]$Server  = "",
    [string]$BuildNumber = "",
    [string]$GitBranch = "",
    [switch]$Installer = $false,
    [switch]$CodeSign = $false
)

# Setup
$ErrorActionPreference = "Stop"
$scriptsPath = Split-Path $Script:MyInvocation.MyCommand.Path
Import-Module "$scriptsPath\common.psm1" -DisableNameChecking

# Get paths
$rootPath = Get-RootPath

# Set build attributes
function Do-PrepareSource()
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
    return 0
}

# Building OpenRCT2
function Do-Build()
{
    Write-Host "Building OpenRCT2..." -ForegroundColor Cyan
    & "$scriptsPath\build.ps1" all -Rebuild
    if ($LASTEXITCODE -ne 0)
    {
        Write-Host "Failed to build OpenRCT2" -ForegroundColor Red
        return 1
    }

    if ($CodeSign)
    {
        $releaseDir = "$rootPath\bin"
        $exePath    = "$releaseDir\openrct2.exe"
        $dllPath    = "$releaseDir\openrct2.dll"

        if (-not (Sign-Binary($exePath))) { return 1 }
        if (-not (Sign-Binary($dllPath))) { return 1 }
    }

    return 0
}

# Package
function Do-Package()
{
    Write-Host "Publishing OpenRCT2 as zip..." -ForegroundColor Cyan
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
    Copy-Item -Force          "$distDir\changelog.txt"         $tempDir -ErrorAction Stop
    Copy-Item -Force          "$distDir\known_issues.txt"      $tempDir -ErrorAction Stop
    Copy-Item -Force          "$distDir\readme.txt"            $tempDir -ErrorAction Stop
    Copy-Item -Force          "$rootPath\contributors.md"      $tempDir -ErrorAction Stop
    Copy-Item -Force          "$rootPath\licence.txt"          $tempDir -ErrorAction Stop

    # Create archive using 7z (renowned for speed and compression)
    $7zcmd = "7za"
    if (-not (AppExists($7zcmd)))
    {
        # AppVeyor in particular uses '7z' instead
        $7zcmd = "7z"
        if (-not (AppExists($7zcmd)))
        {
            Write-Host "Publish script requires 7z to be in PATH" -ForegroundColor Red
            return 1
        }
    }
    & $7zcmd a -tzip -mx9 $outZip "$tempDir\*" | Write-Host
    if ($LASTEXITCODE -ne 0)
    {
        Write-Host "Failed to create zip." -ForegroundColor Red
        return 1
    }

    # Remove temp directory
    Remove-Item -Force -Recurse $tempDir -ErrorAction SilentlyContinue
    return 0
}

# Installer
function Do-Installer()
{
    Write-Host "Publishing OpenRCT2 as installer..." -ForegroundColor Cyan
    $artifactsDir = "$rootPath\artifacts"
    $installerDir = "$rootPath\distribution\windows"

    # Create artifacts directory
    New-Item -Force -ItemType Directory $artifactsDir > $null

    # Create installer
    $GitCommitSha1Short = (git rev-parse --short HEAD)
    $VersionExtra       = "$GitBranch-$GitCommitSha1Short"
    & "$installerDir\build.ps1" -VersionExtra $VersionExtra
    if ($LASTEXITCODE -ne 0)
    {
        Write-Host "Failed to create installer." -ForegroundColor Red
        if (Test-Path -PathType Leaf "$installerDir\win32.log")
        {
            Get-Content "$installerDir\win32.log" | Write-Host
        }
        return 1
    }

    $binaries = (Get-ChildItem "$installerDir\*.exe" | Sort-Object -Property LastWriteTime -Descending)
    if ($binaries -eq 0)
    {
        Write-Host "Unable to find created installer." -ForegroundColor Red
        return 1
    }

    $installerPath = $binaries[0].FullName

    if ($CodeSign)
    {
        if (-not (Sign-Binary($installerPath))) { return 1 }
    }

    Move-Item -Force $installerPath $artifactsDir
    return 0
}

function Do-Task-Build()
{
    if (($result = (Do-PrepareSource)) -ne 0) { return $result }
    if (($result = (Do-Build        )) -ne 0) { return $result }
    return 0
}

function Do-Task-Package()
{
    if ($Installer)
    {
        if (($result = (Do-Installer)) -ne 0) { return $result }
    }
    else
    {
        if (($result = (Do-Package)) -ne 0) { return $result }
    }
    return 0
}

function Do-Task-All()
{
    if (($result = (Do-Task-Build  )) -ne 0) { return $result }
    if (($result = (Do-Task-Package)) -ne 0) { return $result }
    return 0
}

function Sign-Binary($binaryPath)
{
    $pfxPath      = "$rootPath\distribution\windows\code-sign-key-openrct2.org.pfx"
    $pfxPassword  = ${env:CODE-SIGN-KEY-OPENRCT2.ORG.PFX.PASSWORD}
    $timestampUrl = "http://timestamp.comodoca.com/authenticode"

    if (-not (Test-Path -PathType Leaf $pfxPath))
    {
        Write-Host "Unable to sign, code signature key was not found." -ForegroundColor Red
        return 1
    }

    if ($pfxPassword -eq $null)
    {
        Write-Host "Unable to sign, %CODE-SIGN-KEY-OPENRCT2.ORG.PFX.PASSWORD% was not set." -ForegroundColor Red
        return 1
    }

    # Resolve signtool path
    $signtoolcmd = "signtool"
    if (-not (AppExists($signtoolcmd)))
    {
        $signtoolcmd = "C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Bin\SignTool.exe"
        if (-not (AppExists($signtoolcmd)))
        {
            Write-Host "Publish script requires signtool to be in PATH" -ForegroundColor Red
            return 1
        }
    }

    # Sign the binary
    & $signtoolcmd sign /f $pfxPath /p $pfxPassword /t $timestampUrl $binaryPath
    if ($LASTEXITCODE -ne 0)
    {
        Write-Host "Failed to sign binary." -ForegroundColor Red
        return 1
    }

    return 0
}

# Script entry point
switch ($Task)
{
    "build"   { $result = Do-Task-Build   }
    "package" { $result = Do-Task-Package }
    "all"     { $result = Do-Task-All     }
    default   { Write-Host "Unknown publish task." -ForegroundColor Red
                $result = 1 }
}
exit $result
