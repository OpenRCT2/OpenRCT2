param (
    [Parameter(Position = 1)]
    [string]$VersionExtra = ""
)

$path = Split-Path $Script:MyInvocation.MyCommand.Path
Write-Host "Building Windows Installer (NSIS script)";
Write-Host "  $VersionExtra";
makensis   /DAPPV_EXTRA=-$VersionExtra        `
           /DVERSION_INCLUDE=$path\win32.txt  `
         $path\install.nsi > $path\win32.log;
