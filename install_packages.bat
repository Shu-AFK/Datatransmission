@echo off
SET vcpkg_dir=%1
SET install_vcpkg_flag=%2
SET packages=(lz4 libsodium sqlite3)

IF "%install_vcpkg_flag%"=="-install" (
    echo Cloning vcpkg repository and building vcpkg...
    git clone https://github.com/microsoft/vcpkg.git %vcpkg_dir%
    cd %vcpkg_dir%
    .\bootstrap-vcpkg.bat
)

cd %vcpkg_dir%

FOR %%p IN %packages% DO (
    echo Checking package %%p
    vcpkg list | findstr /C:%%p >nul
    IF ERRORLEVEL 1 (
        echo Installing package %%p
        ./vcpkg install %%p
    ) ELSE (
        echo Package %%p is already installed
    )
)
pause