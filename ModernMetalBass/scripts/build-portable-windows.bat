@echo off
setlocal

set "PROJECT_DIR=%~dp0.."
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "BUNDLE=%PROJECT_DIR%\build-win\ModernMetalBass.vst3"
set "BINARY=%BUNDLE%\Contents\x86_64-win\ModernMetalBass.vst3"
set "RELEASE_DIR=%PROJECT_DIR%\release-windows-portable"
set "PORTABLE_DIR=%RELEASE_DIR%\PlayTempo-ModernMetalBass-0.4.1-Windows-x64-Portable"
set "ZIP_PATH=%RELEASE_DIR%\PlayTempo-ModernMetalBass-0.4.1-Windows-x64-Portable.zip"

if not exist "%VSWHERE%" (
  echo ERROR: Visual Studio 2022 or vswhere.exe was not found.
  echo Install Visual Studio 2022 with Desktop development with C++.
  exit /b 1
)

for /f "usebackq tokens=*" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set "VS_INSTALL=%%I"
)

if not defined VS_INSTALL (
  echo ERROR: The Visual Studio C++ toolchain was not found.
  exit /b 1
)

call "%VS_INSTALL%\VC\Auxiliary\Build\vcvars64.bat"
if errorlevel 1 exit /b 1

echo Building Modern Metal Bass 0.4.1 VST3 for Windows x64...
msbuild "%PROJECT_DIR%\projects\ModernMetalBass-vst3.vcxproj" /t:Rebuild /p:Configuration=Release /p:Platform=x64 /m /nologo
if errorlevel 1 (
  echo ERROR: The VST3 build failed.
  exit /b 1
)

if not exist "%BINARY%" (
  echo ERROR: The expected VST3 binary was not created:
  echo %BINARY%
  exit /b 1
)

if exist "%PORTABLE_DIR%" rmdir /s /q "%PORTABLE_DIR%"
if exist "%ZIP_PATH%" del /q "%ZIP_PATH%"
mkdir "%PORTABLE_DIR%"
if errorlevel 1 exit /b 1

xcopy /E /I /H /Y "%BUNDLE%" "%PORTABLE_DIR%\ModernMetalBass.vst3" >nul
if errorlevel 1 (
  echo ERROR: The VST3 bundle could not be copied.
  exit /b 1
)

copy /Y "%PROJECT_DIR%\installer\readme-win-portable.txt" "%PORTABLE_DIR%\INSTALL-WINDOWS.txt" >nul
if errorlevel 1 exit /b 1

copy /Y "%PROJECT_DIR%\LICENSE" "%PORTABLE_DIR%\LICENSE.txt" >nul
if errorlevel 1 exit /b 1
copy /Y "%PROJECT_DIR%\THIRD_PARTY-iPlug2-LICENSE.txt" "%PORTABLE_DIR%\THIRD_PARTY-iPlug2-LICENSE.txt" >nul
if errorlevel 1 exit /b 1
copy /Y "%PROJECT_DIR%\resources\fonts\LICENSE-Roboto.txt" "%PORTABLE_DIR%\LICENSE-Roboto.txt" >nul
if errorlevel 1 exit /b 1
powershell -NoProfile -ExecutionPolicy Bypass -Command "Compress-Archive -Path '%PORTABLE_DIR%\*' -DestinationPath '%ZIP_PATH%' -Force"
if errorlevel 1 (
  echo ERROR: The portable ZIP could not be created.
  exit /b 1
)

echo.
echo Portable package created successfully:
echo %ZIP_PATH%
echo.
echo Testers should extract the ZIP and copy ModernMetalBass.vst3 to:
echo C:\Program Files\Common Files\VST3\
exit /b 0
