@echo off
setlocal

set "PROJECT_DIR=%~dp0.."
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "ISCC=%ProgramFiles(x86)%\Inno Setup 6\ISCC.exe"

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

echo Building DualBassPreamp 1.0.0 VST3 for Windows x64...
msbuild "%PROJECT_DIR%\projects\DualBassPreamp-vst3.vcxproj" /t:Rebuild /p:Configuration=Release /p:Platform=x64 /m /nologo
if errorlevel 1 (
  echo ERROR: The VST3 build failed.
  exit /b 1
)

if not exist "%PROJECT_DIR%\build-win\DualBassPreamp.vst3\Contents\x86_64-win\DualBassPreamp.vst3" (
  echo ERROR: The expected VST3 bundle was not created.
  exit /b 1
)

if not exist "%ISCC%" (
  echo ERROR: Inno Setup 6 was not found.
  echo Install Inno Setup 6, then run this script again.
  exit /b 1
)

echo Creating the Windows installer...
"%ISCC%" "%PROJECT_DIR%\installer\DualBassPreamp.iss"
if errorlevel 1 (
  echo ERROR: The installer build failed.
  exit /b 1
)

echo.
echo Windows installer created successfully:
echo %PROJECT_DIR%\release-windows\PlayTempo-DualBassPreamp-1.0.0-Windows-x64.exe
exit /b 0
