#init
$libversion = 4
$path = Split-Path $Script:MyInvocation.MyCommand.Path
$zip = $path+'\orctlibs.zip'
$libs = $path+'\lib'
$libsVFile = $path+'\libversion'
$liburl = 'https://openrct2.website/files/orctlibs-vs.zip'
$libsTest = Test-Path $libs

#libs version test
$libsVersionTest = Test-Path $libsVFile
$currentVersion = 0
$needsdownload = $true
if ($libsVersionTest) {
    $currentVersion = [IO.File]::ReadAllText($libsVFile)
}
if ($currentVersion -ge $libversion) {
    $needsdownload = $false
}

#download
if (!$libsTest -or $needsdownload) {
	if ($libsTest) {
        rm $libs -Recurse -Force
    }
    mkdir $libs
	Invoke-WebRequest $liburl -OutFile $path\orctlibs.zip
	[System.Reflection.Assembly]::LoadWithPartialName('System.IO.Compression.FileSystem') > $null
	[System.IO.Compression.ZipFile]::ExtractToDirectory($zip, $libs)
	rm $path\orctlibs.zip -Force -ErrorAction SilentlyContinue
    $libversion | Set-Content $libsVFile
} else {
    echo 'All libs up to date'
}
