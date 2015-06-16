$path = Split-Path $Script:MyInvocation.MyCommand.Path
Write-Output "Building Windows Installer (NSIS script)";
makensis /DVERSION_INCLUDE=$path\win32.txt $path\install.nsi > $path\win32.log;