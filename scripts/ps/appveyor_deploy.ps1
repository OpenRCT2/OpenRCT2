###########################################
# Script to deploy OpenRCT2 from AppVeyor #
###########################################

$nottesting = (${env:Configuration} -notlike "*tests")
# Only deploy from VS2017 for now.
$notvs2015 = (${env:APPVEYOR_JOB_NAME} -notlike "*2015*")
if ($nottesting -and $notvs2015)
{
    # Check if OpenRCT2.org API security token is available
    if (${env:OPENRCT2_ORG_TOKEN})
    {
        # Only upload tagged builds, develop branch or push/ branches
        if (${env:APPVEYOR_REPO_TAG} -eq "true" -or ${env:APPVEYOR_REPO_BRANCH} -match "^develop$|^push/")
        {
            msbuild openrct2.proj /t:UploadArtifacts /logger:"C:\Program Files\AppVeyor\BuildAgent\Appveyor.MSBuildLogger.dll"
            # curl is sometimes aliased so be explicit
            $zipPath = (Get-ChildItem artifacts/openrct2-symbols-*.zip).FullName
            Write-Host "Uploading $zipPath to backtrace.io..."
            curl.exe --data-binary "@$zipPath" 'https://openrct2.sp.backtrace.io:6098/post?format=symbols&token=e9e6d681fafdeac9f6131b4b59a155d54bebad567a8c0380d70643f4414819f5'
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
