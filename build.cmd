@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
msbuild openrct2.proj /p:Configuration=Release /p:Platform=x64 /m
