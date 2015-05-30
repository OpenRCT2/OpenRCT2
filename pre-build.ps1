$path = Split-Path $Script:MyInvocation.MyCommand.Path
$zip = $path+'\lib\orcalibs.zip'
$libs = $path+'\lib'
$libcurl = Test-Path $path\lib\libcurl\
$jansson = Test-Path $path\lib\jansson\
$sdl = Test-Path $path\lib\sdl\
if (!$libcurl -or !$jansson -or !$sdl) {
	Invoke-WebRequest http://misozmiric.com/ted/openrct2/orcalibs-vs.zip -OutFile $path\lib\orcalibs.zip
	rm $path\lib\libcurl -r -Force -ErrorAction SilentlyContinue
	rm $path\lib\jansson -r -Force -ErrorAction SilentlyContinue
	rm $path\lib\sdl -r -Force -ErrorAction SilentlyContinue
	[System.Reflection.Assembly]::LoadWithPartialName('System.IO.Compression.FileSystem') > $null
	[System.IO.Compression.ZipFile]::ExtractToDirectory($zip, $libs)
	rm $path\lib\orcalibs.zip -Force -ErrorAction SilentlyContinue
}
