@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=amd64 -host_arch=amd64
if errorlevel 1 exit /b 1
cd /d "C:\Projects\CompetitiveOpenRCT2\OpenRCT2"
msbuild openrct2.proj /p:Configuration=Release /p:Platform=x64 /m /nologo /v:minimal
exit /b %errorlevel%
