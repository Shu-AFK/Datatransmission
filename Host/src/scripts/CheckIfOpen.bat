@echo off
SET PORT_NUMBER=%~1
SET RULE_NAME=TCP Port %PORT_NUMBER%

netsh advfirewall firewall show rule name=all | findstr /C:"LocalPort:%PORT_NUMBER% " /C:"Enabled:Yes" /C:"Action:Allow" /C:"Direction:In"

IF %ERRORLEVEL% equ 1 (
    exit /b 0
) ELSE (
    exit /b 1
)