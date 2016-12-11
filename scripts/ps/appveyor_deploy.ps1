###########################################
# Script to deploy OpenRCT2 from AppVeyor #
###########################################

$testing = (${env:Configuration} -like "*tests")
# Only deploy from VS2015 for now.
$vs2015 = (${env:APPVEYOR_JOB_NAME} -like "*2015*")
if (-not $testing -and $vs2015)
{
    # Check if OpenRCT2.org API security token is available
    if (${env:OPENRCT2_ORG_TOKEN})
    {
        # Only upload tagged builds, develop branch or push/ branches
        if (${env:APPVEYOR_REPO_TAG} -eq "true" -or ${env:APPVEYOR_REPO_BRANCH} -match "^develop$|^push/")
        {
            msbuild openrct2.proj /t:UploadArtifacts /logger:"C:\Program Files\AppVeyor\BuildAgent\Appveyor.MSBuildLogger.dll"
        }
        else
        {
            Write-Host "No deployment: Non-tagged or push branch." -ForegroundColor Yellow
        }
    }
    else
    {
        Write-Host "No deployment: %OPENRCT2_ORG_TOKEN% not available." -ForegroundColor Yellow
    }
}
