@echo off

rem Invokes a tool within a Visual Studio prompt
rem Uses %PLATFORM% to set architecture of prompt

set "vspath="

rem Try to use vswhere to find the latest Visual Studio
set "vswhere=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%vswhere%" set "vswhere=%ProgramFiles%\Microsoft Visual Studio\Installer\vswhere.exe"

if exist "%vswhere%" (
    for /f "usebackq tokens=*" %%i in (`"%vswhere%" -latest -prerelease -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
        set "vspath=%%i"
    )
)

if not defined vspath (
    rem Fallback to searching common paths
    if exist "%ProgramFiles%\Microsoft Visual Studio\2026\Enterprise" (
        set "vspath=%ProgramFiles%\Microsoft Visual Studio\2026\Enterprise"
    ) else if exist "%ProgramFiles%\Microsoft Visual Studio\2026\Community" (
        set "vspath=%ProgramFiles%\Microsoft Visual Studio\2026\Community"
    ) else if exist "%ProgramFiles%\Microsoft Visual Studio\2026\Professional" (
        set "vspath=%ProgramFiles%\Microsoft Visual Studio\2026\Professional"
    ) else if exist "%ProgramFiles%\Microsoft Visual Studio\18\Enterprise" (
        rem Visual Studio 2026 changed its installation path to use "18" instead of "2026"
        set "vspath=%ProgramFiles%\Microsoft Visual Studio\18\Enterprise"
    ) else if exist "%ProgramFiles%\Microsoft Visual Studio\18\Community" (
        set "vspath=%ProgramFiles%\Microsoft Visual Studio\18\Community"
    ) else if exist "%ProgramFiles%\Microsoft Visual Studio\18\Professional" (
        set "vspath=%ProgramFiles%\Microsoft Visual Studio\18\Professional"
    ) else if exist "%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise" (
        set "vspath=%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise"
    ) else if exist "%ProgramFiles%\Microsoft Visual Studio\2022\Community" (
        set "vspath=%ProgramFiles%\Microsoft Visual Studio\2022\Community"
    ) else if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Enterprise" (
        set "vspath=%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Enterprise"
    ) else if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community" (
        set "vspath=%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community"
    )
)

if not defined vspath goto notfound
if not exist "%vspath%" goto notfound

echo Using Visual Studio from %vspath%

if "%platform%"=="x64" (
    call "%vspath%\Common7\Tools\VsDevCmd.bat" -no_logo -arch=x64
) else (
    call "%vspath%\Common7\Tools\VsDevCmd.bat" -no_logo
)

%*
exit /b %errorlevel%

:notfound
echo Visual Studio directory not found
exit /b 1
