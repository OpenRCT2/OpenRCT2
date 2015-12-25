param (
    [Parameter(Position = 1)]
    [string]$BuildNumber = "",
    [string]$GitBranch = ""
)

$path = Split-Path $Script:MyInvocation.MyCommand.Path
Write-Host "Building Windows Installer (NSIS script)";
makensis /DAPPV_BUILD=$BuildNumber /DAPPV_EXTRA=-$GitBranch-b$BuildNumber /DVERSION_INCLUDE=$path\win32.txt $path\install.nsi > $path\win32.log;
