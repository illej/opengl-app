@echo off

set VERSION=2.0.12

if not exist external (
    mkdir external\include
    mkdir external\lib\x64
)

echo installing SDL %VERSION%
echo   downloading
curl --show-error --progress-bar https://www.libsdl.org/release/SDL2-devel-%VERSION%-VC.zip -o sdl2.zip

echo   extracting
tar -xf sdl2.zip

echo   copying files
robocopy SDL2-%VERSION%\include external\include\SDL2 > nul
robocopy SDL2-%VERSION%\lib\x64 external\lib\x64 > nul
copy external\lib\x64\SDL2.dll .

echo   cleaning up
rmdir /Q /S SDL2-%VERSION% > nul
del external\lib\x64\SDL2.dll
del sdl2.zip

echo done