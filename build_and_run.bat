@echo off
cd CodefestAdAstra2024/build
cmake --build .
if %errorlevel% neq 0 exit /b %errorlevel%
cd ..
build\CodefestAdAstra2024.exe %*
pause

