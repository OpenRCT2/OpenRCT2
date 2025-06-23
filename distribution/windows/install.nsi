Unicode True
!define APPNAME             "OpenRCT2"
!define APPVERSION          "${APPV_MAIN}${APPV_EXTRA}"
!define APPVERSIONINTERNAL  "${APPV_MAIN}.0"
!define APPNAMEANDVERSION   "${APPNAME} ${APPVERSION}"
!define APPURLLINK          "https://openrct2.io"
!define OPENRCT2_EXE        "openrct2.exe"
!define OPENRCT2_COM        "openrct2.com"

!if "${PLATFORM}" == "Win32"
    !define APPBITS         32
    !define APPARCH         "win32"

    InstallDir "$PROGRAMFILES32\OpenRCT2\"
!else
    !define APPBITS         64
    !define APPARCH         "win64"

    InstallDir "$PROGRAMFILES64\OpenRCT2\"
!endif

; Define root variable relative to installer
!define PATH_ROOT "..\..\"
!define BINARY_DIR      "${PATH_ROOT}bin"

!define MUI_ICON "${PATH_ROOT}resources\logo\icon.ico"
!define MUI_UNICON "${PATH_ROOT}resources\logo\icon.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "welcome.bmp"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "top.bmp"

BrandingText "$(lsBrandingText)"
SetCompressor LZMA

; Version Info
VIProductVersion "${APPVERSIONINTERNAL}"
VIAddVersionKey "ProductVersion" "${APPVERSION}"
VIAddVersionKey "InternalName" "InstOpenRCT2-${APPARCH}"
VIAddVersionKey "FileVersion" "${APPVERSION}-${APPARCH}"
VIAddVersionKey "LegalCopyright" " "
; Main Install settings
Name "${APPNAMEANDVERSION}"

; NOTE: Keep trailing backslash!
InstallDirRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "Install Folder"
!ifndef OUTFILE
    !define OUTFILE "openrct2-install.exe"
!endif
OutFile "${OUTFILE}"
CRCCheck force

ShowInstDetails show
ShowUninstDetails show

RequestExecutionLevel user

Var SHORTCUTS

; Modern interface settings
!include "MUI2.nsh"
!include "InstallOptions.nsh"
!include "WinVer.nsh"
!include "x64.nsh"
!include "UAC.nsh"

!define MUI_ABORTWARNING
!define MUI_WELCOMEPAGE_TITLE_3LINES
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\..\licence.txt"

!define MUI_COMPONENTSPAGE_SMALLDESC
!insertmacro MUI_PAGE_COMPONENTS

!define MUI_PAGE_CUSTOMFUNCTION_LEAVE DoNotInstallInRCT2Folder
!insertmacro MUI_PAGE_DIRECTORY

ManifestDPIAware true

;Start Menu Folder Page Configuration
!define MUI_STARTMENUPAGE_DEFAULTFOLDER $SHORTCUTS
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKEY_LOCAL_MACHINE"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Shortcut Folder"

!insertmacro MUI_PAGE_STARTMENU "OpenRCT2" $SHORTCUTS

!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_TITLE_3LINES
!define MUI_FINISHPAGE_RUN_TEXT "$(lsRunNow)"
!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_FUNCTION PageFinishRun
!define MUI_FINISHPAGE_LINK "$(lsFinishPageLink)"
!define MUI_FINISHPAGE_LINK_LOCATION "${APPURLLINK}"
!define MUI_FINISHPAGE_NOREBOOTSUPPORT
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\readme.txt"
!define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED
!define MUI_WELCOMEFINISHPAGE_CUSTOMFUNCTION_INIT DisableBack

!insertmacro MUI_PAGE_FINISH
!define MUI_PAGE_HEADER_TEXT "$(lsUninstall)"
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Set languages (first is default language). Other languages are sorted by ISO code.
; Only specify languages for which we actually have translations, because otherwise strings will show up blank.
!insertmacro MUI_LANGUAGE "English"
;!insertmacro MUI_LANGUAGE "Arabic"
;!insertmacro MUI_LANGUAGE "Catalan"
;!insertmacro MUI_LANGUAGE "Czech"
;!insertmacro MUI_LANGUAGE "Danish"
!insertmacro MUI_LANGUAGE "German"
;!insertmacro MUI_LANGUAGE "Esperanto"
;!insertmacro MUI_LANGUAGE "Spanish"
!insertmacro MUI_LANGUAGE "Finnish"
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_LANGUAGE "Galician"
!insertmacro MUI_LANGUAGE "Hungarian"
!insertmacro MUI_LANGUAGE "Italian"
;!insertmacro MUI_LANGUAGE "Japanese"
!insertmacro MUI_LANGUAGE "Korean"
;!insertmacro MUI_LANGUAGE "Norwegian"
!insertmacro MUI_LANGUAGE "Dutch"
;!insertmacro MUI_LANGUAGE "Polish"
!insertmacro MUI_LANGUAGE "PortugueseBR"
!insertmacro MUI_LANGUAGE "Russian"
;!insertmacro MUI_LANGUAGE "Swedish"
;!insertmacro MUI_LANGUAGE "Turkish"
;!insertmacro MUI_LANGUAGE "Ukrainian"
;!insertmacro MUI_LANGUAGE "Vietnamese"
;!insertmacro MUI_LANGUAGE "SimpChinese"
;!insertmacro MUI_LANGUAGE "TradChinese"
!insertmacro MUI_RESERVEFILE_LANGDLL

!include "language\install_en.nsi"
!include "language\install_de.nsi"
!include "language\install_fi.nsi"
!include "language\install_fr.nsi"
!include "language\install_gl.nsi"
!include "language\install_hu.nsi"
!include "language\install_it.nsi"
!include "language\install_ko.nsi"
!include "language\install_nl.nsi"
!include "language\install_pt.nsi"
!include "language\install_ru.nsi"

!macro Init thing
uac_tryagain:
!insertmacro UAC_RunElevated
${Switch} $0
${Case} 0
	${IfThen} $1 = 1 ${|} Quit ${|} ;we are the outer process, the inner process has done its work, we are done
	${IfThen} $3 <> 0 ${|} ${Break} ${|} ;we are admin, let the show go on
	${If} $1 = 3 ;RunAs completed successfully, but with a non-admin user
		MessageBox mb_YesNo|mb_IconExclamation|mb_TopMost|mb_SetForeground "$(lsUACRequiresElevation)" /SD IDNO IDYES uac_tryagain IDNO 0
	${EndIf}
	;fall-through and die
${Case} 1223
	MessageBox mb_IconStop|mb_TopMost|mb_SetForeground "$(lsUACError1223)"
	Quit
${Case} 1062
	MessageBox mb_IconStop|mb_TopMost|mb_SetForeground "$(lsUACError1062)"
	Quit
${Default}
	MessageBox mb_IconStop|mb_TopMost|mb_SetForeground "$(lsUACErrorDefault)"
	Quit
${EndSwitch}

SetShellVarContext all
!macroend

Function PageFinishRun
!insertmacro UAC_AsUser_ExecShell "" "$INSTDIR\${OPENRCT2_EXE}" "" "" ""
FunctionEnd

;--------------------------------------------------------------
; (Core) OpenRCT2 install section. Copies all internal game data
Section "!OpenRCT2" Section1
    ; Make sure to be upgraded OpenRCT2 is not running
    Call CheckOpenRCT2Running

    ; Overwrite files by default, but don't complain on failure
    SetOverwrite try

    SetShellVarContext all

    ; Copy data files. Clear out the old dir first, to ensure upgrades do not result in old objects sticking around.
    RMDir /r "$INSTDIR\data"
    SetOutPath "$INSTDIR\data\"
    File /r ${PATH_ROOT}bin\data\*

    ; Copy the rest of the stuff
    SetOutPath "$INSTDIR\"

    ; Copy text files
    File ..\changelog.txt
    Push "$INSTDIR\changelog.txt"
    Call unix2dos
    File ..\..\licence.txt
    Push "$INSTDIR\licence.txt"
    Call unix2dos
    File ..\readme.txt
    Push "$INSTDIR\readme.txt"
    Call unix2dos
    File ..\..\contributors.md
    Push "$INSTDIR\contributors.md"
    Call unix2dos
    File ..\..\PRIVACY.md
    Push "$INSTDIR\PRIVACY.md"
    Call unix2dos
    File ..\scripting.md
    Push "$INSTDIR\scripting.md"
    Call unix2dos
    File ..\openrct2.d.ts
    Push "$INSTDIR\openrct2.d.ts"
    Call unix2dos

    ; Copy executable
    File /oname=${OPENRCT2_EXE} ${BINARY_DIR}\${OPENRCT2_EXE}
    File /oname=${OPENRCT2_COM} ${BINARY_DIR}\${OPENRCT2_COM}

    ; Create the Registry Entries
    WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "Comments" "Visit ${APPURLLINK}"
    WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "DisplayIcon" "$INSTDIR\${OPENRCT2_EXE},0"
    WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "DisplayName" "OpenRCT2 ${APPVERSION}"
    WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "DisplayVersion" "${APPVERSION}"
    WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "HelpLink" "${APPURLLINK}"
    WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "Install Folder" "$INSTDIR"
    WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "Publisher" "OpenRCT2"
    WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "Shortcut Folder" "$SHORTCUTS"
    WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "UninstallString" "$INSTDIR\uninstall.exe"
    WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "URLInfoAbout" "${APPURLLINK}"
    ; This key sets the Version DWORD that new installers will check against
    WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "Version" "${APPVERSIONINTERNAL}"

    !insertmacro MUI_STARTMENU_WRITE_BEGIN "OpenRCT2"
    CreateShortCut "$DESKTOP\OpenRCT2.lnk" "$INSTDIR\${OPENRCT2_EXE}"
    CreateDirectory "$SMPROGRAMS\$SHORTCUTS"
    CreateShortCut "$SMPROGRAMS\$SHORTCUTS\OpenRCT2.lnk" "$INSTDIR\${OPENRCT2_EXE}"
    CreateShortCut "$SMPROGRAMS\$SHORTCUTS\OpenRCT2-verbose.lnk" "%WINDIR%\System32\cmd.exe" '/C "$INSTDIR\${OPENRCT2_COM}" --verbose'
    CreateShortCut "$SMPROGRAMS\$SHORTCUTS\Uninstall.lnk" "$INSTDIR\uninstall.exe"
    CreateShortCut "$SMPROGRAMS\$SHORTCUTS\Readme.lnk" "$INSTDIR\Readme.txt"
    CreateShortCut "$SMPROGRAMS\$SHORTCUTS\Changelog.lnk" "$INSTDIR\Changelog.txt"
    CreateShortCut "$SMPROGRAMS\$SHORTCUTS\Contributors.lnk" "$INSTDIR\contributors.md"
    CreateShortCut "$SMPROGRAMS\$SHORTCUTS\Privacy Policy.lnk" "$INSTDIR\PRIVACY.md"
    !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

;-------------------------------------------
; Install the uninstaller (option is hidden)
Section -FinishSection
    WriteUninstaller "$INSTDIR\uninstall.exe"
SectionEnd

; Modern install component descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${Section1} "$(lsComponentDescription)"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;-----------------------------------------------
; Uninstall section, deletes all installed files
Section "Uninstall"
    SetShellVarContext all

    ; Remove from registry...
    !insertmacro MUI_STARTMENU_GETFOLDER "OpenRCT2" $SHORTCUTS
    ReadRegStr $SHORTCUTS HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "Shortcut Folder"

    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2"

    ; Delete self
    Delete "$INSTDIR\uninstall.exe"

    ; Delete Shortcuts
    Delete "$DESKTOP\OpenRCT2.lnk"
    Delete "$SMPROGRAMS\$SHORTCUTS\OpenRCT2.lnk"
    Delete "$SMPROGRAMS\$SHORTCUTS\OpenRCT2-verbose.lnk"
    Delete "$SMPROGRAMS\$SHORTCUTS\Uninstall.lnk"
    Delete "$SMPROGRAMS\$SHORTCUTS\Readme.lnk"
    Delete "$SMPROGRAMS\$SHORTCUTS\Changelog.lnk"
    Delete "$SMPROGRAMS\$SHORTCUTS\Contributors.lnk"
    Delete "$SMPROGRAMS\$SHORTCUTS\Privacy Policy.lnk"

    ; Clean up OpenRCT2 dir
    Delete "$INSTDIR\changelog.txt"
    Delete "$INSTDIR\licence.txt"
    Delete "$INSTDIR\readme.txt"
    Delete "$INSTDIR\contributors.md"
    Delete "$INSTDIR\PRIVACY.md"
    Delete "$INSTDIR\scripting.md"
    Delete "$INSTDIR\openrct2.d.ts"
    Delete "$INSTDIR\${OPENRCT2_EXE}"
    Delete "$INSTDIR\${OPENRCT2_COM}"
    Delete "$INSTDIR\INSTALL.LOG"

    ; Data files
    RMDir /r "$INSTDIR\data"

    ; Remove remaining directories
    RMDir "$SMPROGRAMS\$SHORTCUTS"
    RMDir "$INSTDIR"

SectionEnd

;-------------------------------------------------------------------------------
; Determine windows version, returns "win9x" if Win9x/Me/2000/XP SP2- or "winnt" for the rest on the stack
Function GetWindowsVersion
    ClearErrors
    StrCpy $R0 "win9x"

    ${If} ${RunningX64}
        goto WinNT
    ${EndIf}

    ${If} ${IsNT}
        ${If} ${IsWinXP}
        ${AndIf} ${AtLeastServicePack} 3
        ${OrIf} ${AtLeastWin2003}
            GoTo WinNT
        ${EndIf}
    ${EndIf}
    GoTo Done
WinNT:
    StrCpy $R0 "winnt"
Done:
    Push $R0
FunctionEnd

;-------------------------------------------------------------------------------
; Check whether we're not running an installer for 64 bits on 32 bits and vice versa
Function CheckProcessorArchitecture
    ClearErrors
    ${If} ${RunningX64}
        IntCmp ${APPBITS} 64 Done 0
        MessageBox MB_YESNO|MB_ICONINFORMATION "$(ls32bitVersionOn64bitOS)" IDYES Done IDNO Abort
    ${Else}
        IntCmp ${APPBITS} 64 0 Done
        MessageBox MB_YESNO|MB_ICONSTOP "$(ls64bitVersionOn32bitOS)" IDYES Done IDNO Abort
    ${EndIf}
    GoTo Done
Abort:
    Quit
Done:
FunctionEnd

;-------------------------------------------------------------------------------
; Check whether we're not running an installer for NT on 9x and vice versa
Function CheckWindowsVersion
    Call GetWindowsVersion
    Pop $R0
    StrCmp $R0 "win9x" 0 WinNT
    ClearErrors
    StrCmp ${APPARCH} "win9x" Done 0
    MessageBox MB_YESNO|MB_ICONSTOP "$(lsNewWindowsVersionOnOldWindows)" IDYES Done IDNO Abort
    GoTo Done
WinNT:
    ClearErrors
    StrCmp ${APPARCH} "win9x" 0 Done
    MessageBox MB_YESNO|MB_ICONEXCLAMATION "$(lsOldWindowsVersionOnNewWindows)" IDYES Done IDNO Abort
Abort:
    Quit
Done:
FunctionEnd

;-------------------------------------------------------------------------------
; Check whether OpenRCT2 is running
Function CheckOpenRCT2Running
    IfFileExists "$INSTDIR\${OPENRCT2_EXE}" 0 Done
Retry:
    FindProcDLL::FindProc "${OPENRCT2_EXE}"
    Pop $R0
    IntCmp $R0 1 0 Done
    ClearErrors
    Delete "$INSTDIR\${OPENRCT2_EXE}"
    IfErrors 0 Done
    ClearErrors
    MessageBox MB_RETRYCANCEL|MB_ICONEXCLAMATION "$(lsOpenRCT2IsRunning)" IDRETRY Retry
    Abort
Done:
FunctionEnd

;-------------------------------------------------------------------------------
; strips all CRs
; and then converts all LFs into CRLFs
; (this is roughly equivalent to "cat file | dos2unix | unix2dos")
;
; usage:
;    Push "infile"
;    Call unix2dos
;
; beware that this function destroys $0 $1 $2
Function unix2dos
    ClearErrors

    Pop $2
    Rename $2 $2.U2D
    FileOpen $1 $2 w

    FileOpen $0 $2.U2D r

    Push $2 ; save name for deleting

    IfErrors unix2dos_done

    ; $0 = file input (opened for reading)
    ; $1 = file output (opened for writing)

unix2dos_loop:
    ; read a byte (stored in $2)
    FileReadByte $0 $2
    IfErrors unix2dos_done ; EOL
    ; skip CR
    StrCmp $2 13 unix2dos_loop
    ; if LF write an extra CR
    StrCmp $2 10 unix2dos_cr unix2dos_write

unix2dos_cr:
    FileWriteByte $1 13

unix2dos_write:
    ; write byte
    FileWriteByte $1 $2
    ; read next byte
    Goto unix2dos_loop

unix2dos_done:
    ; close files
    FileClose $0
    FileClose $1

    ; delete original
    Pop $0
    Delete $0.U2D

FunctionEnd

;-----------------------------------------------------------------------------------
; Properly compare 2 versions
; syntax:
;    ${VersionCompare} "[Version1]" "[Version2]" $var
; output:
;    $var=0  Versions are equal
;    $var=1  Version1 is newer
;    $var=2  Version2 is newer
Function VersionCompare
    !define VersionCompare `!insertmacro VersionCompareCall`

    !macro VersionCompareCall _VER1 _VER2 _RESULT
        Push `${_VER1}`
        Push `${_VER2}`
        Call VersionCompare
        Pop ${_RESULT}
    !macroend

    Exch $1
    Exch
    Exch $0
    Exch
    Push $2
    Push $3
    Push $4
    Push $5
    Push $6
    Push $7

    begin:
    StrCpy $2 -1
    IntOp $2 $2 + 1
    StrCpy $3 $0 1 $2
    StrCmp $3 '' +2
    StrCmp $3 '.' 0 -3
    StrCpy $4 $0 $2
    IntOp $2 $2 + 1
    StrCpy $0 $0 '' $2

    StrCpy $2 -1
    IntOp $2 $2 + 1
    StrCpy $3 $1 1 $2
    StrCmp $3 '' +2
    StrCmp $3 '.' 0 -3
    StrCpy $5 $1 $2
    IntOp $2 $2 + 1
    StrCpy $1 $1 '' $2

    StrCmp $4$5 '' equal

    StrCpy $6 -1
    IntOp $6 $6 + 1
    StrCpy $3 $4 1 $6
    StrCmp $3 '0' -2
    StrCmp $3 '' 0 +2
    StrCpy $4 0

    StrCpy $7 -1
    IntOp $7 $7 + 1
    StrCpy $3 $5 1 $7
    StrCmp $3 '0' -2
    StrCmp $3 '' 0 +2
    StrCpy $5 0

    StrCmp $4 0 0 +2
    StrCmp $5 0 begin newer2
    StrCmp $5 0 newer1
    IntCmp $6 $7 0 newer1 newer2

    StrCpy $4 '1$4'
    StrCpy $5 '1$5'
    IntCmp $4 $5 begin newer2 newer1

    equal:
    StrCpy $0 0
    goto end
    newer1:
    StrCpy $0 1
    goto end
    newer2:
    StrCpy $0 2

    end:
    Pop $7
    Pop $6
    Pop $5
    Pop $4
    Pop $3
    Pop $2
    Pop $1
    Exch $0
FunctionEnd


Var OLDVERSION
Var UninstallString

;-----------------------------------------------------------------------------------
; NSIS Initialize function, determine if we are going to install/upgrade or uninstall
Function .onInit
    !insertmacro Init "installer"
    StrCpy $SHORTCUTS "OpenRCT2"

    SectionSetFlags 0 17

ShowWelcomeMessage:
    IfSilent FinishCallback
    ReadRegStr $R8 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "Version"
    IfErrors FinishCallback

WelcomeToSetup:
    ; An older version was found.  Let's let the user know there's an upgrade that will take place.
    ReadRegStr $OLDVERSION HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "DisplayVersion"
    ; Gets the older version then displays it in a message box
    MessageBox MB_OK|MB_ICONINFORMATION "$(lsUpgradeNotice)"
    Goto FinishCallback

FinishCallback:
    ClearErrors
    ; Call CheckProcessorArchitecture
    ; Call CheckWindowsVersion
FunctionEnd

Function un.onInit
!insertmacro Init "uninstaller"
FunctionEnd

Function DoNotInstallInRCT2Folder
    IfFileExists "$INSTDIR\Data\g1.dat" datag1exists datag1notexists
    datag1exists:
    MessageBox MB_OK|MB_ICONSTOP "$(lsCannotInstallToRCT2Dir)"
    Abort
    datag1notexists:
    IfFileExists "$INSTDIR\Assets\g1.dat" assetsg1exists assetsg1notexists
    assetsg1exists:
    MessageBox MB_OK|MB_ICONSTOP "$(lsCannotInstallToRCTClassicDir)"
    Abort
    assetsg1notexists:
FunctionEnd
; eof
