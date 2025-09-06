;--------------------------------
; Include Modern UI

!include "MUI2.nsh"

;--------------------------------
; General Configuration

!define APP_VERSION "1.0.0-rc5"
!define APP_VERSION_META "1.0.0.0"
!define APP_NAME "Leapify"

!define INSTALL_DIR "$PROGRAMFILES64\${APP_NAME}"
!define LICENSE_FILE "../dist/LICENSE.txt"
!define FILES_DIR "../dist/"

Name "${APP_NAME}"
OutFile "${APP_NAME}_Installer.exe"
InstallDir "${INSTALL_DIR}"
InstallDirRegKey HKLM "Software\${APP_NAME}\Main" ""
RequestExecutionLevel admin
ShowInstDetails show

VIProductVersion "${APP_VERSION_META}"
VIAddVersionKey /LANG=1033 "ProductName" "${APP_NAME}"
VIAddVersionKey /LANG=1033 "FileDescription" "${APP_NAME} Installer"
VIAddVersionKey /LANG=1033 "LegalCopyright" "Copyright © 2025 Nyabsi"
VIAddVersionKey /LANG=1033 "FileVersion" "${APP_VERSION_META}"
VIAddVersionKey /LANG=1033 "ProductVersion" "${APP_VERSION}"

;--------------------------------
; Variables

Var alreadyInstalled

;--------------------------------
; Interface Settings

!define MUI_ABORTWARNING

;--------------------------------
; Pages

!insertmacro MUI_PAGE_LICENSE "${LICENSE_FILE}"
!define MUI_PAGE_CUSTOMFUNCTION_PRE dirPre
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
; Language

!insertmacro MUI_LANGUAGE "English"

;--------------------------------
; Functions

Function dirPre
    StrCmp $alreadyInstalled "true" 0 +2
        Abort
FunctionEnd

Function .onInit
    StrCpy $alreadyInstalled "false"

    ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "UninstallString"
    StrCmp $R0 "" done

    MessageBox MB_YESNOCANCEL|MB_ICONQUESTION \
        "${APP_NAME} is already installed.$\n$\nClick YES to Repair$\nClick NO to Remove$\nClick CANCEL to abort installation" \
        IDYES repair \
        IDNO remove
    Abort

    repair:
        StrCpy $alreadyInstalled "true"
        Goto done

    remove:
        ExecWait '"$INSTDIR\Uninstall.exe" /S _?=$INSTDIR'
        MessageBox MB_OK "${APP_NAME} has been uninstalled."
        Delete "$INSTDIR\Uninstall.exe"
        RMDir "$INSTDIR"
        Quit

    done:
FunctionEnd


;--------------------------------
; Installer Section

Section "Install" SecInstall

    StrCmp $alreadyInstalled "true" 0 noupgrade
        DetailPrint "Cleaning previous installation..."
        ExecWait '"$INSTDIR\Uninstall.exe" /S _?=$INSTDIR'
        Delete "$INSTDIR\Uninstall.exe"
    noupgrade:

    SetOutPath "$INSTDIR"

    File /r "${FILES_DIR}\leapify\*"
    File "${FILES_DIR}\LICENSE.txt"

    WriteRegStr HKLM "Software\${APP_NAME}\Main" "" $INSTDIR
    WriteUninstaller "$INSTDIR\Uninstall.exe"

    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "DisplayName" "${APP_NAME}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "UninstallString" "$\"$INSTDIR\Uninstall.exe$\""

    ExecWait '"C:\Program Files (x86)\Steam\steamapps\common\SteamVR\bin\win64\vrpathreg.exe" adddriver "$INSTDIR"'

SectionEnd

;--------------------------------
; Uninstaller Section

Section "Uninstall"

    SetOutPath "$INSTDIR"
    RMDir /r "$INSTDIR"

    DeleteRegKey HKLM "Software\${APP_NAME}"
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"
    Delete "$SMPROGRAMS\${APP_NAME}.lnk"

    ExecWait '"C:\Program Files (x86)\Steam\steamapps\common\SteamVR\bin\win64\vrpathreg.exe" removedriver "$INSTDIR"'
SectionEnd