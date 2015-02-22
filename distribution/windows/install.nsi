# Version numbers to update
!define APPV_MAJOR 0
!define APPV_MINOR 2
!define APPV_MAINT 0
!define APPV_BUILD 0
!define APPV_EXTRA "-beta"

!define APPNAME "OpenRCT2"   ; Define application name
!define APPVERSION "${APPV_MAJOR}.${APPV_MINOR}.${APPV_MAINT}${APPV_EXTRA}"  ; Define application version
!define APPVERSIONINTERNAL "${APPV_MAJOR}.${APPV_MINOR}.${APPV_MAINT}.${APPV_BUILD}" ; Define application version in X.X.X.X
!define INSTALLERVERSION ${APPV_MAJOR}${APPV_MINOR}${APPV_MAINT}${APPV_BUILD}
!include ${VERSION_INCLUDE}

!define APPURLLINK "https://github.com/IntelOrca/OpenRCT2"
!define APPNAMEANDVERSION "${APPNAME} ${APPVERSION}"

; Define root variable relative to installer
!define PATH_ROOT "..\..\"

!define MUI_ICON "${PATH_ROOT}resources\logo\icon.ico"
!define MUI_UNICON "${PATH_ROOT}resources\logo\icon.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "welcome.bmp"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "top.bmp"

BrandingText "OpenRCT2 Installer"
SetCompressor LZMA

; Version Info
VIProductVersion "${APPVERSIONINTERNAL}"
VIAddVersionKey "ProductName" "OpenRCT2 ${APPBITS}-bit Installer for Windows ${EXTRA_VERSION}"
VIAddVersionKey "Comments" "Installs ${APPNAMEANDVERSION}"
VIAddVersionKey "CompanyName" "OpenRCT2 Developers"
VIAddVersionKey "FileDescription" "Installs ${APPNAMEANDVERSION}"
VIAddVersionKey "ProductVersion" "${APPVERSION}"
VIAddVersionKey "InternalName" "InstOpenRCT2-${APPARCH}"
VIAddVersionKey "FileVersion" "${APPVERSION}-${APPARCH}"
VIAddVersionKey "LegalCopyright" " "
; Main Install settings
Name "${APPNAMEANDVERSION} ${APPBITS}-bit for Windows ${EXTRA_VERSION}"

; NOTE: Keep trailing backslash!
InstallDirRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "Install Folder"
OutFile "openrct2-${APPVERSION}-${APPARCH}.exe"
CRCCheck force

ShowInstDetails show
ShowUninstDetails show

RequestExecutionLevel admin

Var SHORTCUTS

; Modern interface settings
!include "MUI2.nsh"
!include "InstallOptions.nsh"
!include "WinVer.nsh"
!include "x64.nsh"

!define MUI_ABORTWARNING
!define MUI_WELCOMEPAGE_TITLE_3LINES
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\..\licence.txt"

!define MUI_COMPONENTSPAGE_SMALLDESC
!insertmacro MUI_PAGE_COMPONENTS

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
!define MUI_FINISHPAGE_RUN_TEXT "Run ${APPNAMEANDVERSION} now!"
!define MUI_FINISHPAGE_RUN "$INSTDIR\openrct2.exe"
!define MUI_FINISHPAGE_LINK "Visit the OpenRCT2 site for more information"
!define MUI_FINISHPAGE_LINK_LOCATION "${APPURLLINK}"
!define MUI_FINISHPAGE_NOREBOOTSUPPORT
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\readme.txt"
!define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED
!define MUI_WELCOMEFINISHPAGE_CUSTOMFUNCTION_INIT DisableBack

!insertmacro MUI_PAGE_FINISH
!define MUI_PAGE_HEADER_TEXT "Uninstall ${APPNAMEANDVERSION}"
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Set languages (first is default language)
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_RESERVEFILE_LANGDLL

;--------------------------------------------------------------
; (Core) OpenRCT2 install section. Copies all internal game data
Section "!OpenRCT2" Section1
    ; Make sure to be upgraded OpenRCT2 is not running
    Call CheckOpenRCT2Running

    ; Overwrite files by default, but don't complain on failure
    SetOverwrite try

    SetShellVarContext all

    ; Copy language files
    SetOutPath "$INSTDIR\data\language\"
    File ${PATH_ROOT}build\Release\Data\Language\*.txt

    ; Copy data files
    SetOutPath "$INSTDIR\data\"
    File /r ${PATH_ROOT}build\Release\Data\*

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

    ; Copy executable
    File /oname=openrct2.exe ${BINARY_DIR}\openrct2.exe
    File /oname=openrct2.dll ${BINARY_DIR}\openrct2.dll
    File /oname=SDL2.dll ${BINARY_DIR}\SDL2.dll

    ; Create the Registry Entries
    WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "Comments" "Visit ${APPURLLINK}"
    WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "DisplayIcon" "$INSTDIR\openrct2.exe,0"
    WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "DisplayName" "OpenRCT2 ${APPVERSION}"
    WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "DisplayVersion" "${APPVERSION}"
    WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "HelpLink" "${APPURLLINK}"
    WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "Install Folder" "$INSTDIR"
    WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "Publisher" "OpenRCT2"
    WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "Shortcut Folder" "$SHORTCUTS"
    WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "UninstallString" "$INSTDIR\uninstall.exe"
    WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "URLInfoAbout" "${APPURLLINK}"
    ; This key sets the Version DWORD that new installers will check against
    WriteRegDWORD HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "Version" ${INSTALLERVERSION}

    !insertmacro MUI_STARTMENU_WRITE_BEGIN "OpenRCT2"
    CreateShortCut "$DESKTOP\OpenRCT2.lnk" "$INSTDIR\openrct2.exe"
    CreateDirectory "$SMPROGRAMS\$SHORTCUTS"
    CreateShortCut "$SMPROGRAMS\$SHORTCUTS\OpenRCT2.lnk" "$INSTDIR\openrct2.exe"
    CreateShortCut "$SMPROGRAMS\$SHORTCUTS\Uninstall.lnk" "$INSTDIR\uninstall.exe"
    CreateShortCut "$SMPROGRAMS\$SHORTCUTS\Readme.lnk" "$INSTDIR\Readme.txt"
    CreateShortCut "$SMPROGRAMS\$SHORTCUTS\Changelog.lnk" "$INSTDIR\Changelog.txt"
    CreateShortCut "$SMPROGRAMS\$SHORTCUTS\Contributors.lnk" "$INSTDIR\contributors.md"
    !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

;-------------------------------------------
; Install the uninstaller (option is hidden)
Section -FinishSection
    WriteUninstaller "$INSTDIR\uninstall.exe"
SectionEnd

; Modern install component descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${Section1} "Minimal OpenRCT2 installation in English. You must have RollerCoaster Tycoon 2 installed."
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
    Delete "$SMPROGRAMS\$SHORTCUTS\Uninstall.lnk"
    Delete "$SMPROGRAMS\$SHORTCUTS\Readme.lnk"
    Delete "$SMPROGRAMS\$SHORTCUTS\Changelog.lnk"
    Delete "$SMPROGRAMS\$SHORTCUTS\Contributors.lnk"

    ; Clean up OpenRCT2 dir
    Delete "$INSTDIR\changelog.txt"
    Delete "$INSTDIR\readme.txt"
    Delete "$INSTDIR\contributors.md"
    Delete "$INSTDIR\openrct2.exe"
    Delete "$INSTDIR\openrct2.dll"
    Delete "$INSTDIR\SDL2.dll"
    Delete "$INSTDIR\licence.txt"
    Delete "$INSTDIR\INSTALL.LOG"
    Delete "$INSTDIR\crash.log"
    Delete "$INSTDIR\crash.dmp"

    ; Data files
    Delete "$INSTDIR\data\language\*.txt"
    Delete "$INSTDIR\data\title\*.*"

    ; Remove remaining directories
    RMDir "$SMPROGRAMS\$SHORTCUTS"
    RMDir "$INSTDIR\data"
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
        MessageBox MB_YESNO|MB_ICONINFORMATION "You are trying to install the 32-bit OpenRCT2 on a 64-bit operating system. This is not advised, but will work with reduced capabilities. We suggest that you download the correct version. Do you really want to continue?" IDYES Done IDNO Abort
    ${Else}
        IntCmp ${APPBITS} 64 0 Done
        MessageBox MB_YESNO|MB_ICONSTOP "You are trying to install the 64-bit OpenRCT2 on a 32-bit operating system. This is not going to work. Please download the correct version. Do you really want to continue?" IDYES Done IDNO Abort
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
    MessageBox MB_YESNO|MB_ICONSTOP "You are trying to install the Windows XP SP3, Vista, 7 and 8.1 version on Windows 95, 98, ME, 2000 and XP without SP3. This is will not work. Please download the correct version. Do you really want to continue?" IDYES Done IDNO Abort
    GoTo Done
WinNT:
    ClearErrors
    StrCmp ${APPARCH} "win9x" 0 Done
    MessageBox MB_YESNO|MB_ICONEXCLAMATION "You are trying to install the Windows 95, 98, 2000 and XP without SP3 version on Windows XP SP3, Vista, 7 and 8.1. This is not advised, but will work with reduced capabilities. We suggest that you download the correct version. Do you really want to continue?" IDYES Done IDNO Abort
Abort:
    Quit
Done:
FunctionEnd

;-------------------------------------------------------------------------------
; Check whether OpenRCT2 is running
Function CheckOpenRCT2Running
    IfFileExists "$INSTDIR\openrct2.exe" 0 Done
Retry:
    FindProcDLL::FindProc "openrct2.exe"
    Pop $R0
    IntCmp $R0 1 0 Done
    ClearErrors
    Delete "$INSTDIR\openrct2.exe"
    IfErrors 0 Done
    ClearErrors
    MessageBox MB_RETRYCANCEL|MB_ICONEXCLAMATION "OpenRCT2 is running. Please close it and retry." IDRETRY Retry
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


Var OLDVERSION
Var UninstallString

;-----------------------------------------------------------------------------------
; NSIS Initialize function, determine if we are going to install/upgrade or uninstall
Function .onInit
    StrCpy $SHORTCUTS "OpenRCT2"

    SectionSetFlags 0 17

    ; Starts Setup - let's look for an older version of OpenRCT2
    ReadRegDWORD $R8 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "Version"

    IfErrors ShowWelcomeMessage ShowUpgradeMessage
ShowWelcomeMessage:
    ReadRegStr $R8 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "Version"
    IfErrors FinishCallback

ShowUpgradeMessage:
    IntCmp ${INSTALLERVERSION} $R8 VersionsAreEqual InstallerIsOlder  WelcomeToSetup
WelcomeToSetup:
    ; An older version was found.  Let's let the user know there's an upgrade that will take place.
    ReadRegStr $OLDVERSION HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "DisplayVersion"
    ; Gets the older version then displays it in a message box
    MessageBox MB_OK|MB_ICONINFORMATION \
        "Welcome to ${APPNAMEANDVERSION} Setup.$\nThis will allow you to upgrade from version $OLDVERSION."
    Goto FinishCallback

VersionsAreEqual:
    ReadRegStr $UninstallString HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\OpenRCT2" "UninstallString"
    IfFileExists "$UninstallString" "" FinishCallback
    MessageBox MB_YESNO|MB_ICONQUESTION \
        "Setup detected ${APPNAMEANDVERSION} on your system. This is the same version that this program will install.$\nAre you trying to uninstall it?" \
        IDYES DoUninstall IDNO FinishCallback
DoUninstall: ; You have the same version as this installer.  This allows you to uninstall.
    Exec "$UninstallString"
    Quit

InstallerIsOlder:
    MessageBox MB_OK|MB_ICONSTOP \
        "You have a newer version of ${APPNAME}.$\nSetup will now exit."
    Quit

FinishCallback:
    ClearErrors
    ; Call CheckProcessorArchitecture
    ; Call CheckWindowsVersion
FunctionEnd
; eof
