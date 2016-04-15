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
