#########################################################
# Script to build OpenRCT2 on AppVeyor
#########################################################

function Push-Build($file, $name, $version, $flavourId)
{
    curl.exe -s -o -                                       `
         --form "key=${env:OPENRCT2.ORG_TOKEN}"            `
         --form "fileName=$name"                           `
         --form "version=$version"                         `
         --form "gitHash=${env:APPVEYOR_REPO_COMMIT}"      `
         --form "gitBranch=${env:APPVEYOR_REPO_BRANCH}"    `
         --form "flavourId=$flavourId"                     `
         --form "file=@$file"                              `
         "https://openrct2.org/altapi/?command=push-build"
}

# Provide a short commit SHA1 too
${env:APPVEYOR_REPO_COMMIT_SHORT} = (${env:APPVEYOR_REPO_COMMIT}).Substring(0, 7)

# Install dependencies
install -Quiet

# Build OpenRCT2
publish build `
        -Server      AppVeyor                   `
        -BuildNumber $env:APPVEYOR_BUILD_NUMBER `
        -GitBranch   $env:APPVEYOR_REPO_BRANCH  `
        -CodeSign

if ($LASTEXITCODE -ne 0)
{
    exit 1
}

# Create a Portable ZIP
publish package                                 `
        -Server      AppVeyor                   `
        -BuildNumber $env:APPVEYOR_BUILD_NUMBER `
        -GitBranch   $env:APPVEYOR_REPO_BRANCH

# Create an Installer
publish package                                 `
        -Installer                              `
        -Server      AppVeyor                   `
        -BuildNumber $env:APPVEYOR_BUILD_NUMBER `
        -GitBranch   $env:APPVEYOR_REPO_BRANCH  `
        -CodeSign

$version      = "0.0.4.0"
$pushFileName = "OpenRCT2-$version-windows-${env:APPVEYOR_REPO_COMMIT_SHORT}"

# Push portable zip
Write-Host "Sending portable zip to OpenRCT2.org" -ForegroundColor Cyan
Push-Build -file      ".\artifacts\openrct2.zip" `
           -name      "$pushFileName.zip"        `
           -version   $version                   `
           -flavourId 1

Write-Host "Sending installer to OpenRCT2.org"    -ForegroundColor Cyan
Push-Build -file      ".\artifacts\openrct2-install.exe" `
           -name      "$pushFileName.exe"                `
           -version   $version                           `
           -flavourId 2
