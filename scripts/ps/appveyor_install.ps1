#########################################################
# Script to setup OpenRCT2 for building on AppVeyor
#########################################################

if ($env:ENCKEY)
{
    Write-Host "Downloading secure-file from NuGet" -ForegroundColor Cyan
    nuget install secure-file -ExcludeVersion
    secure-file\tools\secure-file -decrypt distribution\windows\code-sign-key-openrct2.org.pfx.enc -secret $env:ENCKEY
}

if (${env:OPENRCT2.ORG_TOKEN})
{
    Write-Host "Downloading NSIS from chocolatey"   -ForegroundColor Cyan
    cinst nsis.portable --version=3.01-beta1 > $null

    Write-Host "Downloading KillProcDLL for NSIS"   -ForegroundColor Cyan
    curl "http://nsis.sourceforge.net/mediawiki/images/5/53/KillProcDll%26FindProcDll.zip" -OutFile nsisxtra.zip
    7z x nsisxtra.zip > $null
    cp FindProcDLL.dll "C:\ProgramData\chocolatey\lib\nsis.portable\tools\nsis-3.0b1\Plugins\x86-ansi"
}
