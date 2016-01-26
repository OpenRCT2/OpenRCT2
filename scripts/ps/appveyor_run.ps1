#########################################################
# Script to build OpenRCT2 on AppVeyor
#########################################################

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
