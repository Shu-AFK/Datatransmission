@echo off
SET PORT_NUMBER=27015
SET RULE_NAME=TCP Port %PORT_NUMBER%

netsh advfirewall firewall add rule name="%RULE_NAME%" dir=in action=allow protocol=TCP localport=%PORT_NUMBER%
if %ERRORLEVEL% equ 0 (
   exit /b 0) else (
   exit /b 1
)