@echo off
setlocal
cd /d "%~dp0"

echo ============================================================
echo  Bloom - Clean
echo ============================================================
echo.

echo Removing build artifacts...

if exist bloom.exe (
    del /f bloom.exe
    echo   deleted bloom.exe
)

if exist dist\ (
    rmdir /s /q dist
    echo   deleted dist\
)

if exist *.test (
    del /f *.test
    echo   deleted *.test
)

if exist *.out (
    del /f *.out
    echo   deleted *.out
)

echo.
echo Done. Working tree is clean.
echo.
