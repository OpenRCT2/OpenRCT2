$scriptsPath = Split-Path $Script:MyInvocation.MyCommand.Path
$rootPath = Split-Path (Split-Path $scriptsPath)
&("$rootPath\bin\openrct2.exe") $args
