@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64 -host_arch=amd64
if errorlevel 1 exit /b 1
cd /d "%~dp0"
cl /nologo /std:c++20 /EHsc /O2 /I"..\lib\x64\include" competitive-master-server.cpp /Fe:competitive-master-server.exe /link ws2_32.lib
exit /b %errorlevel%
