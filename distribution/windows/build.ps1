Write-Output "Building Windows Installer (NSIS script)";
makensis /DVERSION_INCLUDE=win32.txt install.nsi > win32.log;