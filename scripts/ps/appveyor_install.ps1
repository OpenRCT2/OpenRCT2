#########################################################
# Script to setup OpenRCT2 for building on AppVeyor
#########################################################

$testing = (${env:Configuration} -like "*tests")

function Check-ExitCode
{
    if ($LASTEXITCODE -ne 0)
    {
        $host.SetShouldExit($LASTEXITCODE)
    }
}

if ($env:ENCKEY -and -not $testing)
{
    if (-not (Test-Path "secure-file"))
    {
        Write-Host "Downloading secure-file from NuGet..." -ForegroundColor Cyan
        nuget install secure-file -ExcludeVersion
    }
    
    Write-Host "Decrypting code signing key..."            -ForegroundColor Cyan
    secure-file\tools\secure-file -decrypt distribution\windows\code-sign-key-openrct2.org.pfx.enc -secret $env:ENCKEY
    Check-ExitCode
}

# Check if OpenRCT2.org API security token is available
if (${env:OPENRCT2_ORG_TOKEN} -and -not $testing)
{
    if (-not (Test-Path "C:\ProgramData\chocolatey\lib\nsis.portable"))
    {
        Write-Host "Downloading NSIS from chocolatey..."   -ForegroundColor Cyan
        cinst nsis.portable --version=3.01-beta1 > $null
        if ($LASTEXITCODE -ne 0)
        {
            Get-Content "C:\ProgramData\chocolatey\logs\chocolatey.log"
            $host.SetShouldExit(1)
        }

        Write-Host "Downloading KillProcDLL for NSIS..."   -ForegroundColor Cyan
        curl "http://nsis.sourceforge.net/mediawiki/images/5/53/KillProcDll%26FindProcDll.zip" -OutFile nsisxtra.zip
        Check-ExitCode
        7z x nsisxtra.zip > $null
        Check-ExitCode
        cp FindProcDLL.dll "C:\ProgramData\chocolatey\lib\nsis.portable\tools\nsis-3.0b1\Plugins\x86-ansi"

        Write-Host "Downloading UAC plugin for NSIS..."   -ForegroundColor Cyan
        curl "http://nsis.sourceforge.net/mediawiki/images/8/8f/UAC.zip" -OutFile uac.zip
        Check-ExitCode
        7z x uac.zip > $null
        Check-ExitCode
        cp UAC.nsh "C:\ProgramData\chocolatey\lib\nsis.portable\tools\nsis-3.0b1\Include"
        cp Plugins "C:\ProgramData\chocolatey\lib\nsis.portable\tools\nsis-3.0b1" -Recurse -Force
    }
}
else
{
    # Don't build the NSIS installer for non-uploaded builds
    ${env:NO_NSIS} = "true"
}

$env:GIT_TAG = $env:APPVEYOR_REPO_TAG_NAME
if (${env:APPVEYOR_REPO_TAG} -ne "true")
{
    $env:GIT_BRANCH = $env:APPVEYOR_REPO_BRANCH
}
$env:GIT_COMMIT_SHA1 = $env:APPVEYOR_REPO_COMMIT
