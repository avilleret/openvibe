  !include "MUI.nsh"
  !include "zipdll.nsh"

  ;Name and file
  Name "OpenViBE dependencies"
  OutFile "win32-install_dependencies.exe"

  ;Default installation folder
  InstallDir "$EXEDIR\..\dependencies"

;Interface Settings

  !define MUI_ABORTWARNING

;Pages

  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_INSTFILES

;  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;Languages

  !insertmacro MUI_LANGUAGE "English"

;Installer and uninstaller icons

  Icon "${NSISDIR}\Contrib\Graphics\Icons\box-install.ico"
  UninstallIcon "${NSISDIR}\Contrib\Graphics\Icons\box-uninstall.ico"

;##########################################################################################################################################################
;##########################################################################################################################################################
;##########################################################################################################################################################

Section "-base"

  ;Finds Microsoft Platform SDK

  ReadRegStr $r0 HKLM "SOFTWARE\Microsoft\Win32SDK\Directories" "Install Dir"
  StrCmp $r0 "" base_failed_to_find_sdk_1 base_found_sdk
base_failed_to_find_sdk_1:
  ReadRegStr $r0 HKLM "SOFTWARE\Microsoft\MicrosoftSDK\Directories" "Install Dir"
  StrCmp $r0 "" base_failed_to_find_sdk_2 base_found_sdk
base_failed_to_find_sdk_2:
  ReadRegStr $r0 HKLM "SOFTWARE\Microsoft\Microsoft SDKs\Windows" "CurrentInstallFolder"
  StrCmp $r0 "" base_failed_to_find_sdk_3 base_found_sdk
base_failed_to_find_sdk_3:
  goto base_failed_to_find_sdk

base_failed_to_find_sdk:
  MessageBox MB_OK|MB_ICONEXCLAMATION "Failed to find Microsoft Platform SDK$\nPlease update your win32-dependencies.cmd script by hand"
  goto base_go_on
base_found_sdk:
  MessageBox MB_OK "Microsoft Platform SDK found at :$\n$r0"
  goto base_go_on

base_go_on:

  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  ;clears dependencies file
  FileOpen $0 "$EXEDIR\win32-dependencies.cmd" w
  FileWrite $0 "@echo off$\r$\n"
  FileWrite $0 "$\r$\n"
  FileWrite $0 "SET OV_DEP_MSSDK=$r0$\r$\n"
  FileClose $0

SectionEnd

;##########################################################################################################################################################
;##########################################################################################################################################################
;##########################################################################################################################################################

Section "CMake"

  SetOutPath "$INSTDIR"
  CreateDirectory "$INSTDIR\arch"

  IfFileExists "arch\openvibe-dependency-cmake-2.6.2.zip" no_need_to_download_cmake
  NSISdl::download http://www.irisa.fr/bunraku/OpenViBE/dependencies/win32/cmake-2.6.2-win32-x86.zip "arch\openvibe-dependency-cmake-2.6.2.zip"
  Pop $R0 ; Get the return value
    StrCmp $R0 "success" +3
      MessageBox MB_OK "Download failed: $R0"
      Quit

no_need_to_download_cmake:

  IfFileExists "cmake" no_need_to_install_cmake
  ZipDLL::extractall "arch\openvibe-dependency-cmake-2.6.2.zip" "cmake"

no_need_to_install_cmake:

  FileOpen $0 "$EXEDIR\win32-dependencies.cmd" a
  FileSeek $0 0 END
  FileWrite $0 "SET OV_DEP_CMAKE=$INSTDIR\cmake\cmake-2.6.2-win32-x86$\r$\n"
  FileClose $0

SectionEnd

;##########################################################################################################################################################
;##########################################################################################################################################################
;##########################################################################################################################################################

Section "eXpat"

  SetOutPath "$INSTDIR"
  CreateDirectory "$INSTDIR\arch"

  IfFileExists "arch\openvibe-dependency-expat-2.0.1.zip" no_need_to_download_expat
  NSISdl::download http://www.irisa.fr/bunraku/OpenViBE/dependencies/win32/expat-win32bin-2.0.1.zip "arch\openvibe-dependency-expat-2.0.1.zip"
  Pop $R0 ; Get the return value
    StrCmp $R0 "success" +3
      MessageBox MB_OK "Download failed: $R0"
      Quit

no_need_to_download_expat:

  IfFileExists "expat" no_need_to_install_expat
  ZipDLL::extractall "arch\openvibe-dependency-expat-2.0.1.zip" "expat"

no_need_to_install_expat:

  FileOpen $0 "$EXEDIR\win32-dependencies.cmd" a
  FileSeek $0 0 END
  FileWrite $0 "SET OV_DEP_EXPAT=$INSTDIR\expat$\r$\n"
  FileClose $0

SectionEnd

;##########################################################################################################################################################
;##########################################################################################################################################################
;##########################################################################################################################################################

Section "BOOST"

  SetOutPath "$INSTDIR"
  CreateDirectory "$INSTDIR\arch"

  IfFileExists "arch\openvibe-dependency-boost-1.34.0.zip" no_need_to_download_boost
  NSISdl::download http://www.irisa.fr/bunraku/OpenViBE/dependencies/win32/boost_1_34_0.zip "arch\openvibe-dependency-boost-1.34.0.zip"
  Pop $R0 ; Get the return value
    StrCmp $R0 "success" +3
      MessageBox MB_OK "Download failed: $R0"
      Quit

no_need_to_download_boost:

  IfFileExists "boost" no_need_to_install_boost
  ZipDLL::extractall "arch\openvibe-dependency-boost-1.34.0.zip" "boost"

no_need_to_install_boost:

  FileOpen $0 "$EXEDIR\win32-dependencies.cmd" a
  FileSeek $0 0 END
  FileWrite $0 "SET OV_DEP_BOOST=$INSTDIR\boost\boost_1_34_0$\r$\n"
  FileClose $0

SectionEnd

;##########################################################################################################################################################
;##########################################################################################################################################################
;##########################################################################################################################################################

Section "GTK/Glade"

  SetOutPath "$INSTDIR"
  CreateDirectory "$INSTDIR\arch"

  IfFileExists "arch\openvibe-dependency-gtk-2.10.11.zip" no_need_to_download_glade
  NSISdl::download http://www.irisa.fr/bunraku/OpenViBE/dependencies/win32/gtk-dev-2.10.11-win32-1.zip "arch\openvibe-dependency-gtk-2.10.11.zip"
  Pop $R0 ; Get the return value
    StrCmp $R0 "success" +3
      MessageBox MB_OK "Download failed: $R0"
      Quit

no_need_to_download_glade:

  IfFileExists "GTK" no_need_to_install_glade
  ZipDLL::extractall "arch\openvibe-dependency-gtk-2.10.11.zip" "gtk"

no_need_to_install_glade:

  FileOpen $0 "$INSTDIR\gtk\lib\pkgconfig\libglade-2.0.pc" w
  FileWrite $0 "prefix=$INSTDIR\gtk$\r$\n"
  FileWrite $0 "exec_prefix=$${prefix}$\r$\n"
  FileWrite $0 "libdir=$${exec_prefix}/lib$\r$\n"
  FileWrite $0 "includedir=$${prefix}/include$\r$\n"
  FileWrite $0 "$\r$\n"
  FileWrite $0 "# so people can do $\"pkg-config --variable moduledir libglade-2.0$\"$\r$\n"
  FileWrite $0 "# (which is broken - users can now use the LIBGLADE_MODULE_PATH$\r$\n"
  FileWrite $0 "# environment variable$\r$\n"
  FileWrite $0 "moduledir=$${libdir}/libglade/2.0$\r$\n"
  FileWrite $0 "$\r$\n"
  FileWrite $0 "Name: Libglade$\r$\n"
  FileWrite $0 "Description: a library for dynamically loading GLADE interface files$\r$\n"
  FileWrite $0 "Version: 2.5.1$\r$\n"
  FileWrite $0 "Requires: gtk+-2.0 libxml-2.0$\r$\n"
  FileWrite $0 "Libs: -L$${libdir} -lglade-2.0$\r$\n"
  FileWrite $0 "Cflags: -I$${includedir}/libglade-2.0$\r$\n"
  FileClose $0

  FileOpen $0 "$EXEDIR\win32-dependencies.cmd" a
  FileSeek $0 0 END
  FileWrite $0 "SET OV_DEP_GLADE=$INSTDIR\gtk$\r$\n"
  FileClose $0

SectionEnd

;##########################################################################################################################################################
;##########################################################################################################################################################
;##########################################################################################################################################################

Section "IT++"

  SetOutPath "$INSTDIR"
  CreateDirectory "$INSTDIR\arch"

  IfFileExists "arch\openvibe-dependency-itpp-4.0.6.zip" no_need_to_download_itpp
  NSISdl::download http://www.irisa.fr/bunraku/OpenViBE/dependencies/win32/itpp-4.0.6.zip "arch\openvibe-dependency-itpp-4.0.6.zip"
  Pop $R0 ; Get the return value
    StrCmp $R0 "success" +3
      MessageBox MB_OK "Download failed: $R0"
      Quit

no_need_to_download_itpp:

  IfFileExists "itpp" no_need_to_install_itpp
  ZipDLL::extractall "arch\openvibe-dependency-itpp-4.0.6.zip" "itpp"

no_need_to_install_itpp:

  FileOpen $0 "$EXEDIR\win32-dependencies.cmd" a
  FileSeek $0 0 END
  FileWrite $0 "SET OV_DEP_ITPP=$INSTDIR\itpp$\r$\n"
  FileClose $0

SectionEnd

;##########################################################################################################################################################
;##########################################################################################################################################################
;##########################################################################################################################################################

Section "OBT"

  SetOutPath "$INSTDIR"
  CreateDirectory "$INSTDIR\arch"

  IfFileExists "arch\openvibe-dependency-obt-22.zip" no_need_to_download_obt
  NSISdl::download http://www.irisa.fr/bunraku/OpenViBE/dependencies/win32/obt-22.zip "arch\openvibe-dependency-obt-22.zip"
  Pop $R0 ; Get the return value
    StrCmp $R0 "success" +3
      MessageBox MB_OK "Download failed: $R0"
      Quit

no_need_to_download_obt:

  IfFileExists "obt" no_need_to_install_obt
  ZipDLL::extractall "arch\openvibe-dependency-obt-22.zip" "obt"

no_need_to_install_obt:

  FileOpen $0 "$EXEDIR\win32-dependencies.cmd" a
  FileSeek $0 0 END
  FileWrite $0 "SET OV_DEP_OBT=$INSTDIR\obt$\r$\n"
  FileClose $0

SectionEnd

;##########################################################################################################################################################
;##########################################################################################################################################################
;##########################################################################################################################################################

SectionGroup /e "Ogre3D"
Section /o "Ogre3D for Visual C++ 2003" ogre_vc_2003

  SetOutPath "$INSTDIR"
  CreateDirectory "$INSTDIR\arch"

  IfFileExists "arch\openvibe-dependency-ogre-1.6.2-vc2003.zip" no_need_to_download_ogre_vc2003
  NSISdl::download http://www.irisa.fr/bunraku/OpenViBE/dependencies/win32/ogre-1.6.2-vc2003.zip "arch\openvibe-dependency-ogre-1.6.2-vc2003.zip"
  Pop $R0 ; Get the return value
    StrCmp $R0 "success" +3
      MessageBox MB_OK "Download failed: $R0"
      Quit

no_need_to_download_ogre_vc2003:

  IfFileExists "ogre-vc2003" no_need_to_install_ogre_vc2003
  ZipDLL::extractall "arch\openvibe-dependency-ogre-1.6.2-vc2003.zip" "ogre-vc2003"

no_need_to_install_ogre_vc2003:

  FileOpen $0 "$EXEDIR\win32-dependencies.cmd" a
  FileSeek $0 0 END
  FileWrite $0 "SET OV_DEP_OGRE=$INSTDIR\ogre-vc2003$\r$\n"
  FileWrite $0 "SET OGRE_HOME=$INSTDIR\ogre-vc2003$\r$\n"
  FileClose $0

SectionEnd

Section /o "Ogre3D for Visual C++ 2005" ogre_vc_2005

  SetOutPath "$INSTDIR"
  CreateDirectory "$INSTDIR\arch"

  IfFileExists "arch\openvibe-dependency-ogre-1.6.2-vc2005.zip" no_need_to_download_ogre_vc2005
  NSISdl::download http://www.irisa.fr/bunraku/OpenViBE/dependencies/win32/ogre-1.6.2-vc2005.zip "arch\openvibe-dependency-ogre-1.6.2-vc2005.zip"
  Pop $R0 ; Get the return value
    StrCmp $R0 "success" +3
      MessageBox MB_OK "Download failed: $R0"
      Quit

no_need_to_download_ogre_vc2005:

  IfFileExists "ogre-vc2005" no_need_to_install_ogre_vc2005
  ZipDLL::extractall "arch\openvibe-dependency-ogre-1.6.2-vc2005.zip" "ogre-vc2005"

no_need_to_install_ogre_vc2005:

  FileOpen $0 "$EXEDIR\win32-dependencies.cmd" a
  FileSeek $0 0 END
  FileWrite $0 "SET OV_DEP_OGRE=$INSTDIR\ogre-vc2005$\r$\n"
  FileWrite $0 "SET OGRE_HOME=$INSTDIR\ogre-vc2005$\r$\n"
  FileClose $0

SectionEnd

Section "Ogre3D for Visual C++ 2008" ogre_vc_2008

  SetOutPath "$INSTDIR"
  CreateDirectory "$INSTDIR\arch"

  IfFileExists "arch\openvibe-dependency-ogre-1.6.2-vc2008.zip" no_need_to_download_ogre_vc2008
  NSISdl::download http://www.irisa.fr/bunraku/OpenViBE/dependencies/win32/ogre-1.6.2-vc2008.zip "arch\openvibe-dependency-ogre-1.6.2-vc2008.zip"
  Pop $R0 ; Get the return value
    StrCmp $R0 "success" +3
      MessageBox MB_OK "Download failed: $R0"
      Quit

no_need_to_download_ogre_vc2008:

  IfFileExists "ogre-vc2008" no_need_to_install_ogre_vc2008
  ZipDLL::extractall "arch\openvibe-dependency-ogre-1.6.2-vc2008.zip" "ogre-vc2008"

no_need_to_install_ogre_vc2008:

  FileOpen $0 "$EXEDIR\win32-dependencies.cmd" a
  FileSeek $0 0 END
  FileWrite $0 "SET OV_DEP_OGRE=$INSTDIR\ogre-vc2008$\r$\n"
  FileWrite $0 "SET OGRE_HOME=$INSTDIR\ogre-vc2008$\r$\n"
  FileClose $0

SectionEnd
SectionGroupEnd

;##########################################################################################################################################################
;##########################################################################################################################################################
;##########################################################################################################################################################

Section "OpenMASK 4 / DotSceneLoader"

  SetOutPath "$INSTDIR"
  CreateDirectory "$INSTDIR\arch"

  IfFileExists "arch\openvibe-dependency-openmask4~dotsceneloader-164.zip" no_need_to_download_openmask
  NSISdl::download http://www.irisa.fr/bunraku/OpenViBE/dependencies/win32/openmask4~dotsceneloader-164.zip "arch\openvibe-dependency-openmask4~dotsceneloader-164.zip"
  Pop $R0 ; Get the return value
    StrCmp $R0 "success" +3
      MessageBox MB_OK "Download failed: $R0"
      Quit

no_need_to_download_openmask:

  IfFileExists "openmask" no_need_to_install_openmask
  ZipDLL::extractall "arch\openvibe-dependency-openmask4~dotsceneloader-164.zip" "openmask"

no_need_to_install_openmask:

  FileOpen $0 "$EXEDIR\win32-dependencies.cmd" a
  FileSeek $0 0 END
  FileWrite $0 "SET OV_DEP_OPENMASK=$INSTDIR\openmask$\r$\n"
  FileWrite $0 "SET OV_DEP_DSI=$INSTDIR\openmask$\r$\n"
  FileWrite $0 "SET OMK_HOME=$INSTDIR\openmask$\r$\n"
  FileWrite $0 "SET OMK_BIN=$INSTDIR\openmask$\r$\n"
  FileClose $0

SectionEnd

;##########################################################################################################################################################
;##########################################################################################################################################################
;##########################################################################################################################################################

Section "VRPN"

  SetOutPath "$INSTDIR"
  CreateDirectory "$INSTDIR\arch"

  IfFileExists "arch\openvibe-dependency-vrpn-7.73.zip" no_need_to_download_vrpn
  NSISdl::download http://www.irisa.fr/bunraku/OpenViBE/dependencies/win32/vrpn-7.13.zip "arch\openvibe-dependency-vrpn-7.13.zip"
  Pop $R0 ; Get the return value
    StrCmp $R0 "success" +3
      MessageBox MB_OK "Download failed: $R0"
      Quit

no_need_to_download_vrpn:

  IfFileExists "vrpn" no_need_to_install_vrpn
  ZipDLL::extractall "arch\openvibe-dependency-vrpn-7.13.zip" "vrpn"

no_need_to_install_vrpn:

  FileOpen $0 "$EXEDIR\win32-dependencies.cmd" a
  FileSeek $0 0 END
  FileWrite $0 "SET OV_DEP_VRPN=$INSTDIR\vrpn$\r$\n"
  FileWrite $0 "SET VRPNROOT=$INSTDIR\vrpn$\r$\n"
  FileClose $0

SectionEnd

;##########################################################################################################################################################
;##########################################################################################################################################################
;##########################################################################################################################################################

Section "DirectX"

  SetOutPath "$INSTDIR"
  CreateDirectory "$INSTDIR\arch"

  IfFileExists "$SYSDIR\d3dx9_38.dll" no_need_to_install_directx
  IfFileExists "arch\openvibe-directx.exe" no_need_to_download_directx
  NSISdl::download http://www.microsoft.com/downloads/info.aspx?na=90&p=&SrcDisplayLang=en&SrcCategoryId=&SrcFamilyId=04ac064b-00d1-474e-b7b1-442d8712d553&u=http%3a%2f%2fdownload.microsoft.com%2fdownload%2fB%2f7%2f9%2fB79FC9D7-47B8-48B7-A75E-101DEBEB5AB4%2fdirectx_aug2009_redist.exe "arch\openvibe-directx.exe"
  Pop $R0 ; Get the return value
    StrCmp $R0 "success" +3
      MessageBox MB_OK "Download failed: $R0"
      Quit

no_need_to_download_directx:

  ExecWait '"arch\openvibe-directx.exe" /T:$INSTDIR\tmp /Q'
  ExecWait '"tmp\DXSETUP.exe" /silent'

no_need_to_install_directx:

SectionEnd

;##########################################################################################################################################################
;##########################################################################################################################################################
;##########################################################################################################################################################

Section "Visual Redistributable Package"

  SetOutPath "$INSTDIR"
  CreateDirectory "$INSTDIR\arch"

  ;IfFileExists "$SYSDIR\d3dx9_38.dll" no_need_to_install_vcredist
  IfFileExists "arch\openvibe-vcredist.exe" no_need_to_download_vcredist
  NSISdl::download http://www.microsoft.com/DOWNLOADS/info.aspx?na=90&p=&SrcDisplayLang=en&SrcCategoryId=&SrcFamilyId=9b2da534-3e03-4391-8a4d-074b9f2bc1bf&u=http%3a%2f%2fdownload.microsoft.com%2fdownload%2f1%2f1%2f1%2f1116b75a-9ec3-481a-a3c8-1777b5381140%2fvcredist_x86.exe "arch\openvibe-vcredist.exe"
  Pop $R0 ; Get the return value
    StrCmp $R0 "success" +3
      MessageBox MB_OK "Download failed: $R0"
      Quit

no_need_to_download_vcredist:

  ExecWait '"arch\openvibe-vcredist.exe" /q:a /c:"VCREDI~1.EXE /q:a /c:""msiexec /i vcredist.msi /q!"" "'

no_need_to_install_vcredist:

SectionEnd

;##########################################################################################################################################################
;##########################################################################################################################################################
;##########################################################################################################################################################

Section "Uninstall"

  RMDir /r "$INSTDIR\gtk"
  RMDir /r "$INSTDIR\boost"
  RMDir /r "$INSTDIR\expat"
  RMDir /r "$INSTDIR\cmake"
  RMDir /r "$INSTDIR\itpp"
  RMDir /r "$INSTDIR\obt"
  RMDir /r "$INSTDIR\ogre"
  RMDir /r "$INSTDIR\openmask"
  RMDir /r "$INSTDIR\vrpn"
  RMDir /r "$INSTDIR\tmp"

  Delete "$INSTDIR\..\scripts\win32-dependencies.cmd"

  Delete "$INSTDIR\Uninstall.exe"

  RMDir "$INSTDIR"

SectionEnd

;##########################################################################################################################################################
;##########################################################################################################################################################
;##########################################################################################################################################################

Function .onInit
  StrCpy $1 ${ogre_vc_2008}
FunctionEnd

Function .onSelChange
	!insertmacro StartRadioButtons $1
		!insertmacro RadioButton ${ogre_vc_2003}
		!insertmacro RadioButton ${ogre_vc_2005}
		!insertmacro RadioButton ${ogre_vc_2008}
	!insertmacro EndRadioButtons
FunctionEnd

