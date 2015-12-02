# Publish script for OpenRCT2
# - Sets the source code preprocessor defines
# - Builds a clean release of OpenRCT2
# - Creates a ZIP for distribution

param (
    [string]$server = "",
    [string]$buildNo = ""
)

# Set build attributes
function do-prepareSource($build_server = "", $build_number = "")
{
    Write-Output "Setting build #defines..."
    # $build_number = "";
    # $build_server = "";
    $build_branch = (git rev-parse --abbrev-ref HEAD);
    $build_commit_sha1 = (git rev-parse HEAD);
    $build_commit_sha1_short = $build_commit_sha1.Substring(0, [System.Math]::Min($build_commit_sha1.Length, 7));

    $defines = @{ }
    $defines["OPENRCT2_BUILD_NUMBER"]      = $build_number;
    $defines["OPENRCT2_BUILD_SERVER"]      = $build_server;
    $defines["OPENRCT2_BRANCH"]            = $build_branch;
    $defines["OPENRCT2_COMMIT_SHA1"]       = $build_commit_sha1;
    $defines["OPENRCT2_COMMIT_SHA1_SHORT"] = $build_commit_sha1_short;

    $rct2path = ".\src\rct2.h";
    $lines = Get-Content $rct2path
    $newLines = @( )
    foreach ($line in $lines) {
        if ($line -ne $null -and $line.Contains("#define") -eq $true) {
            foreach ($key in $defines.Keys) {
	            if ($line.Contains($key + "`t") -or $line.Contains($key + " ")) {
                    $line = "#define " + $key + " """ + $defines[$key] + """"
                    break;
                }
            }
        }
        $newLines += $line
    }
    $newLines | Set-Content $rct2path
}

# Building OpenRCT2
function do-build()
{
    Write-Output "Building OpenRCT2..."
    msbuild .\projects\openrct2.sln /p:Configuration=Release /p:Platform=Win32 /t:rebuild /v:minimal
}

# Package
function do-package()
{
    Write-Output "Publishing OpenRCT2..."
    $releaseDir = ".\build\Release"
    $distDir = ".\distribution"
    $tempDir = ".\artifacts\temp"
    $outZip = ".\artifacts\openrct2.zip"

    # Create new temp directory
    Remove-Item -Force -Recurse $tempDir -ErrorAction SilentlyContinue
    New-Item -Force -ItemType Directory $tempDir > $null

    # Copy files to be archived
    Copy-Item -Force "$releaseDir\openrct2.exe"       $tempDir -ErrorAction Stop
    Copy-Item -Force "$releaseDir\openrct2.dll"       $tempDir -ErrorAction Stop
    Copy-Item -Force "$releaseDir\curl-ca-bundle.crt" $tempDir -ErrorAction Stop
    Copy-Item -Force "$releaseDir\SDL2.dll"           $tempDir -ErrorAction Stop
    Copy-Item -Force "$distDir\changelog.txt"         $tempDir -ErrorAction Stop
    Copy-Item -Force "$distDir\known_issues.txt"      $tempDir -ErrorAction Stop
    Copy-Item -Force "$distDir\readme.txt"            $tempDir -ErrorAction Stop

    # Create archive
    7za a -tzip -mx9 $outZip "$tempDir\*"

    # Remove temp directory
    Remove-Item -Force -Recurse $tempDir -ErrorAction SilentlyContinue
}

do-prepareSource $server $buildNo
do-build
do-package
