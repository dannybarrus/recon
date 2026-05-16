@echo off
setlocal

:: Always run from the directory containing this batch file
cd /d "%~dp0"

echo ============================================================
echo  Bloom Filter - Recon
echo ============================================================
echo.

:: Check Go is installed
where go >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: Go is not installed or not in PATH.
    echo        Download from https://golang.org/dl/
    pause
    exit /b 1
)

echo Go version:
go version
echo.

:: Tidy dependencies
echo [1/4] Tidying dependencies...
go mod tidy
if %errorlevel% neq 0 (
    echo ERROR: go mod tidy failed.
    pause
    exit /b 1
)

:: Run tests
echo.
echo [2/4] Running tests...
go test ./...
if %errorlevel% neq 0 (
    echo ERROR: Tests failed.
    pause
    exit /b 1
)
echo Tests passed.

:: Build the binary
echo.
echo [3/4] Building binary...
go build -o bloom.exe ./cmd/bloom
if %errorlevel% neq 0 (
    echo ERROR: Build failed.
    pause
    exit /b 1
)
echo Build successful -- bloom.exe

:: Run the demo
echo.
echo [4/4] Running demo...
echo.
echo --- Demo: 1000 items, 1%% target false positive rate, 10000 non-member checks ---
echo.
echo NOTE: The demo loads items into the filter and immediately checks non-members
echo       against the same filter instance. The standalone add and check commands
echo       each create a fresh filter -- state is not persisted between calls.
echo.
bloom.exe demo --capacity 1000 --fpr 0.01 --trials 10000 --workers 4

echo.
echo --- Demo: larger run, 10000 items ---
echo.
bloom.exe demo --capacity 10000 --fpr 0.01 --trials 50000 --workers 4

echo.
echo ============================================================
echo  Done. Run bloom.exe --help for usage.
echo ============================================================
echo.
pause
