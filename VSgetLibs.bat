@echo OFF
pushd "%~dp0"
IF NOT EXIST .\lib\libcurl\ (
	curl http://misozmiric.com/ted/openrct2/orcalibs-vs.zip -o .\lib\orcalibs.zip
	pushd .\lib
	unzip -oaq orcalibs.zip
	del .\orcalibs.zip
	popd
)
popd