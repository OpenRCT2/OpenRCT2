# OpenRCT2 Competitive Lobby - Build and Test Script
# This script helps set up dependencies and build the project with tests

Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "OpenRCT2 Competitive Lobby Build Script" -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host ""

$projectRoot = "D:\Projects\Git_Hosting\SimCity_MultiPlayer\OpenRCT2_V2"
$buildDir = Join-Path $projectRoot "build"

# Check if vcpkg is installed
Write-Host "Checking for vcpkg..." -ForegroundColor Yellow
$vcpkgPath = $null

# Common vcpkg locations
$vcpkgLocations = @(
    "C:\vcpkg",
    "$env:USERPROFILE\vcpkg",
    "$projectRoot\vcpkg"
)

foreach ($location in $vcpkgLocations) {
    if (Test-Path "$location\vcpkg.exe") {
        $vcpkgPath = $location
        Write-Host "✓ Found vcpkg at: $vcpkgPath" -ForegroundColor Green
        break
    }
}

if (-not $vcpkgPath) {
    Write-Host "✗ vcpkg not found!" -ForegroundColor Red
    Write-Host ""
    Write-Host "To install vcpkg, run:" -ForegroundColor Yellow
    Write-Host "  git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg" -ForegroundColor White
    Write-Host "  cd C:\vcpkg" -ForegroundColor White
    Write-Host "  .\bootstrap-vcpkg.bat" -ForegroundColor White
    Write-Host ""
    Write-Host "Then install dependencies:" -ForegroundColor Yellow
    Write-Host "  .\vcpkg install freetype:x64-windows libpng:x64-windows zlib:x64-windows zstd:x64-windows sdl2:x64-windows openssl:x64-windows icu:x64-windows curl:x64-windows speexdsp:x64-windows" -ForegroundColor White
    Write-Host "  .\vcpkg integrate install" -ForegroundColor White
    Write-Host ""
    exit 1
}

# Check if dependencies are installed
Write-Host ""
Write-Host "Checking dependencies..." -ForegroundColor Yellow

$requiredPackages = @(
    "freetype:x64-windows",
    "libpng:x64-windows", 
    "zlib:x64-windows",
    "zstd:x64-windows",
    "sdl2:x64-windows",
    "openssl:x64-windows",
    "icu:x64-windows"
)

$missingPackages = @()

foreach ($package in $requiredPackages) {
    $installed = & "$vcpkgPath\vcpkg.exe" list $package 2>$null
    if ($installed) {
        Write-Host "  ✓ $package" -ForegroundColor Green
    } else {
        Write-Host "  ✗ $package (missing)" -ForegroundColor Red
        $missingPackages += $package
    }
}

if ($missingPackages.Count -gt 0) {
    Write-Host ""
    Write-Host "Missing dependencies detected!" -ForegroundColor Red
    Write-Host "Install them with:" -ForegroundColor Yellow
    Write-Host "  cd $vcpkgPath" -ForegroundColor White
    Write-Host "  .\vcpkg install $($missingPackages -join ' ')" -ForegroundColor White
    Write-Host ""
    
    $response = Read-Host "Do you want to install missing dependencies now? (y/n)"
    if ($response -eq 'y' -or $response -eq 'Y') {
        Write-Host ""
        Write-Host "Installing dependencies..." -ForegroundColor Yellow
        Push-Location $vcpkgPath
        $packages = $missingPackages -join ' '
        & .\vcpkg.exe install $packages
        Pop-Location
        Write-Host "✓ Dependencies installed!" -ForegroundColor Green
    } else {
        exit 1
    }
}

# Configure CMake
Write-Host ""
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "Configuring CMake..." -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor Cyan

$vcpkgToolchain = Join-Path $vcpkgPath "scripts\buildsystems\vcpkg.cmake"

if (-not (Test-Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir | Out-Null
}

Push-Location $projectRoot

$cmakeArgs = @(
    "-B", "build",
    "-DWITH_TESTS=ON",
    "-DCMAKE_BUILD_TYPE=Debug",
    "-DCMAKE_TOOLCHAIN_FILE=$vcpkgToolchain"
)

Write-Host "Running: cmake $($cmakeArgs -join ' ')" -ForegroundColor White
& cmake $cmakeArgs

if ($LASTEXITCODE -ne 0) {
    Write-Host "✗ CMake configuration failed!" -ForegroundColor Red
    Pop-Location
    exit 1
}

Write-Host "✓ CMake configured successfully!" -ForegroundColor Green

# Build the project
Write-Host ""
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "Building project..." -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor Cyan

& cmake --build build --config Debug -- /maxcpucount

if ($LASTEXITCODE -ne 0) {
    Write-Host "✗ Build failed!" -ForegroundColor Red
    Pop-Location
    exit 1
}

Write-Host "✓ Build completed successfully!" -ForegroundColor Green

# Run tests
Write-Host ""
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "Running tests..." -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor Cyan

Push-Location $buildDir

Write-Host ""
Write-Host "Running all tests..." -ForegroundColor Yellow
& ctest -C Debug --output-on-failure

Write-Host ""
Write-Host "Running Competition Lobby tests specifically..." -ForegroundColor Yellow
if (Test-Path "Debug\OpenRCT2Tests.exe") {
    & .\Debug\OpenRCT2Tests.exe --gtest_filter="CompetitionLobby*"
    
    Write-Host ""
    Write-Host "Running Competition Integration tests..." -ForegroundColor Yellow
    & .\Debug\OpenRCT2Tests.exe --gtest_filter="CompetitionIntegration*"
} else {
    Write-Host "✗ Test executable not found!" -ForegroundColor Red
}

Pop-Location
Pop-Location

Write-Host ""
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "Build and Test Complete!" -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Summary:" -ForegroundColor Yellow
Write-Host "  • Configuration: ✓" -ForegroundColor Green
Write-Host "  • Build: ✓" -ForegroundColor Green
Write-Host "  • Tests: Check output above" -ForegroundColor White
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "  • Review test results above" -ForegroundColor White
Write-Host "  • Run OpenRCT2 to test UI: .\build\Debug\openrct2.exe" -ForegroundColor White
Write-Host "  • See COMPETITIVE_LOBBY_IMPLEMENTATION.md for full documentation" -ForegroundColor White
Write-Host ""
