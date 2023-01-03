@echo off
mkdir Build
pushd Build
cl ../src/main.cpp /nologo /Fe:sprit.exe /I"../dp" /MD /link opengl32.lib user32.lib gdi32.lib ../dp/glfw3.lib kernel32.lib shell32.lib
sprit.exe
popd