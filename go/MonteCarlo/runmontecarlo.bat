@echo off
setlocal

:: Always run from the directory containing this batch file
cd /d "%~dp0"

echo ============================================================
echo  Monte Carlo Simulation Engine - Recon
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
go build -o montecarlo.exe ./cmd/montecarlo
if %errorlevel% neq 0 (
    echo ERROR: Build failed.
    pause
    exit /b 1
)
echo Build successful -- montecarlo.exe

:: Run the simulations
echo.
echo [4/4] Running simulations...

echo.
echo --- All simulations (1 million trials each) ---
echo.
montecarlo.exe all --trials 1000000

echo.
echo --- Pi convergence (watch accuracy improve step by step) ---
echo.
montecarlo.exe convergence --sim pi --max 5000000 --steps 10

echo.
echo --- Euler's number convergence ---
echo.
montecarlo.exe convergence --sim euler --max 5000000 --steps 10

echo.
echo ============================================================
echo  Done. Run montecarlo.exe --help for usage.
echo ============================================================
echo.
pause
