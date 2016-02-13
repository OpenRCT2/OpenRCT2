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

$server = "AppVeyor"

# Provide a short commit SHA1 too
${env:APPVEYOR_REPO_COMMIT_SHORT} = (${env:APPVEYOR_REPO_COMMIT}).Substring(0, 7)

# Current version
$version = "0.0.4.0"

# Tagged builds will hide branch and commit SHA1
$tag = $null
if (${env:APPVEYOR_REPO_TAG} -ne $null)
{
    $tag = ${env:APPVEYOR_REPO_TAG_NAME}
}

# Enable code signing if password environment variable is set
$codeSign = $false
if (${env:CODE-SIGN-KEY-OPENRCT2.ORG.PFX.PASSWORD} -ne $null)
{
    $codeSign = $true
}

# Enable pushing builds to OpenRCT2.org if token environment variable is set
$pushBuilds = $false
$installer  = $false
if (${env:OPENRCT2.ORG_TOKEN} -ne $null)
{
    $pushBuilds = $true
    $installer  = $true
}

# Write out summary of the build
Write-Host "AppVeyor CI Build" -ForegroundColor Green
if ($tag -ne $null)
{
    Write-Host "  $version-$tag" -ForegroundColor Green
}
else
{
    Write-Host "  $version-$env:APPVEYOR_REPO_BRANCH-$env:APPVEYOR_REPO_COMMIT_SHORT" -ForegroundColor Green
}
Write-Host "  Signed: $codeSign"   -ForegroundColor Green
Write-Host "  Push  : $pushBuilds" -ForegroundColor Green

# Install dependencies
install -Quiet

# Build OpenRCT2
publish build `
        -Server       $server                         `
        -GitTag       $tag                            `
        -GitBranch    $env:APPVEYOR_REPO_BRANCH       `
        -GitSha1      $env:APPVEYOR_REPO_COMMIT       `
        -GitSha1Short $env:APPVEYOR_REPO_COMMIT_SHORT `
        -CodeSign     $codeSign

if ($LASTEXITCODE -ne 0)
{
    exit 1
}

# Create a Portable ZIP
publish package                                       `
        -Server       $server                         `
        -GitTag       $tag                            `
        -GitBranch    $env:APPVEYOR_REPO_BRANCH       `
        -GitSha1      $env:APPVEYOR_REPO_COMMIT       `
        -GitSha1Short $env:APPVEYOR_REPO_COMMIT_SHORT `
        -CodeSign     $codeSign

# Create an Installer
if ($installer)
{
    publish package                                       `
            -Installer                                    `
            -Server       $server                         `
            -GitTag       $tag                            `
            -GitBranch    $env:APPVEYOR_REPO_BRANCH       `
            -GitSha1      $env:APPVEYOR_REPO_COMMIT       `
            -GitSha1Short $env:APPVEYOR_REPO_COMMIT_SHORT `
            -CodeSign     $codeSign
}

if ($pushBuilds)
{
    $versionExtension = ""
    if ($tag -ne $null)
    {
        $versionExtension = "-$tag"
    }
    else
    {
        $versionExtension = "-${env:APPVEYOR_REPO_BRANCH}-${env:APPVEYOR_REPO_COMMIT_SHORT}"
    }
    $pushFileName = "OpenRCT2-${version}${versionExtension}-windows"

    # Push portable zip
    Write-Host "Sending portable zip to OpenRCT2.org" -ForegroundColor Cyan
    Push-Build -file      ".\artifacts\openrct2.zip" `
               -name      "$pushFileName.zip"        `
               -version   $version                   `
               -flavourId 1

    # Push installer
    if ($installer)
    {
        Write-Host "Sending installer to OpenRCT2.org"    -ForegroundColor Cyan
        Push-Build -file      ".\artifacts\openrct2-install.exe" `
                   -name      "$pushFileName.exe"                `
                   -version   $version                           `
                   -flavourId 2
    }
}
