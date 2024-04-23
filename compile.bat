@echo off
gcc -o clock.exe clock.c -lgdi32
if %ERRORLEVEL% neq 0 (
    echo Compilation failed.
    pause
) else (
    echo Compilation succeeded.
    echo Running clock.exe...
    start clock.exe
)
