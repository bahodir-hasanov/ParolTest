@echo off
cmake -S . -B build -A x64
if errorlevel 1 (
  cmake -S . -B build -G "MinGW Makefiles"
  if errorlevel 1 exit /b 1
  cmake --build build -j
  echo Built: build\paroltest.exe
  exit /b 0
)
cmake --build build --config Release
echo Built: build\paroltest.exe
