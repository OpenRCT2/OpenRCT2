$path = Split-Path $Script:MyInvocation.MyCommand.Path
$zip = $path+'\orctlibs.zip'
$libs = $path+'\lib'
$libsTest = Test_path $libs
if (!$libsTest) {
	mkdir $libs
	Invoke-WebRequest https://download.openrct2.website/dev/lib/vs -OutFile $path\orctlibs.zip
	[System.Reflection.Assembly]::LoadWithPartialName('System.IO.Compression.FileSystem') > $null
	[System.IO.Compression.ZipFile]::ExtractToDirectory($zip, $libs)
	rm $path\orctlibs.zip -Force -ErrorAction SilentlyContinue
}
