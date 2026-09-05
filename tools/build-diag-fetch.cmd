@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64 -host_arch=amd64
if errorlevel 1 exit /b 1
cd /d "%~dp0"
cl /nologo /std:c++20 /EHsc /I"..\lib\x64\include" diag-fetch.cpp /Fe:diag-fetch.exe /link winhttp.lib
exit /b %errorlevel%
