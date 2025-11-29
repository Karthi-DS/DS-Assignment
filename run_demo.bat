@echo off
title Trie Autocomplete & Spell Checker Demo
color 0B

echo.
echo ========================================
echo    TRIE DEMONSTRATION LAUNCHER
echo ========================================
echo.

REM Check if executable exists
if not exist "trie_demo.exe" (
    echo [ERROR] trie_demo.exe not found!
    echo.
    echo Compiling from source...
    gcc trie_demo.c -o trie_demo.exe -Wall
    if errorlevel 1 (
        echo [ERROR] Compilation failed!
        echo Please ensure GCC is installed and trie_demo.c exists.
        pause
        exit /b 1
    )
    echo [SUCCESS] Compilation complete!
    echo.
)

REM Check if dictionary exists
if not exist "words_alpha.txt" (
    echo [ERROR] words_alpha.txt not found!
    echo Please ensure the dictionary file is in this directory.
    echo.
    pause
    exit /b 1
)

echo [OK] All files present
echo [OK] Starting demo...
echo.
timeout /t 2 /nobreak >nul

REM Run the program
trie_demo.exe

REM After program exits
echo.
echo.
echo ========================================
echo    Thank you!
echo ========================================
echo.
pause
