#########################################################
# Common functions for OpenRCT2 PowerShell scripts
#########################################################
$scriptsPath = Split-Path $Script:MyInvocation.MyCommand.Path

function AppExists($app)
{
    $result = (Get-Command $app -CommandType Application -ErrorAction SilentlyContinue)
    return ($result -ne $null -and $result.Count -gt 0)
}

function AddPath($path)
{
    $env:path = "$path;$env:path"
}

function Get-RootPath()
{
    return Split-Path (Split-Path $scriptsPath)
}

function Prompt-User($message)
{
    $yes = New-Object System.Management.Automation.Host.ChoiceDescription "&Yes", "Yes"
    $no =  New-Object System.Management.Automation.Host.ChoiceDescription "&No", "No"
    $options = [System.Management.Automation.Host.ChoiceDescription[]]($yes, $no)
    $choice = $host.UI.PromptForChoice("", $message, $options, 1)
    return ($choice -eq 0)
}

function Resolve-PathFromBase($path)
{
    $rootPath = Get-RootPath
    if ($path.StartsWith($rootPath)) {
        $path = $path.Remove(0, $rootPath.Length + 1)
    }
    return $path
}

function Symlink-or-Copy($path, $target)
{
    $pathDirectory = Split-Path $path
    $pathName      = Split-Path $path -Leaf

    $friendlyPath   = Resolve-PathFromBase $path
    $friendlyTarget = Resolve-PathFromBase $target

    # If the path is not a symlink, copy files instead
    $mustCopy = $false
    if (Test-Path $path)
    {
        if (-not ((Get-Item $path).Attributes -band [IO.FileAttributes]::ReparsePoint))
        {
            $mustCopy = $true
        }
    }

    $symlinkSuccessful = $false
    if (-not $mustCopy)
    {
        try
        {
            Write-Host "Symlink $friendlyPath to $friendlyTarget..." -ForegroundColor Cyan
            New-Item -Force -ItemType SymbolicLink -Path $pathDirectory -Name $pathName -Target $target -ErrorAction Stop
            $symlinkSuccessful = $true
        }
        catch [System.Management.Automation.ParameterBindingException]
        {
            Write-Host "  Your powershell can not create symlinks, try updating it"             -ForegroundColor Red
        }
        catch [System.UnauthorizedAccessException]
        {
            Write-Host "  You need to run powershell in administration mode to create symlinks" -ForegroundColor Red
        }
    }

    if (-not $symlinkSuccessful)
    {
        Write-Host "Copying $friendlyTarget to $friendlyPath..." -ForegroundColor Cyan
        New-Item  -Force -Type Directory $path > $null
        Copy-Item -Force -Recurse "$target\*" $path
    }
}
