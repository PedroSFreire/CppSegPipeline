@echo off
setlocal

:: Get the directory of the running batch file
set "SCRIPT_DIR=%~dp0"
set "VCPKG_DIR=%SCRIPT_DIR%vcpkg"

:: Move to the script's directory
cd /d "%SCRIPT_DIR%"

:: Check if vcpkg is already installed
if exist "%VCPKG_DIR%" (
    echo vcpkg is already installed in %VCPKG_DIR%
) else (
    echo Installing vcpkg in %SCRIPT_DIR%...
    git clone https://github.com/microsoft/vcpkg.git "%VCPKG_DIR%"
    cd "%VCPKG_DIR%"
    bootstrap-vcpkg.bat
)

:: Ensure vcpkg.exe is in PATH
set "PATH=%VCPKG_DIR%;%PATH%"

:: Install required packages
echo Installing ITK, OpenCV, VLC, and VLC Core...
"%VCPKG_DIR%\vcpkg" install itk:x64-windows
"%VCPKG_DIR%\vcpkg" install opencv:x64-windows
"%VCPKG_DIR%\vcpkg" install glew:x64-windows
"%VCPKG_DIR%\vcpkg" install glfw3:x64-windows

echo All packages installed successfully!
pause
