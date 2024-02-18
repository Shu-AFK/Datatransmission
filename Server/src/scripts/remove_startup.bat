@echo off
set "scriptName=run_exec.bat"
set "target=%appdata%\Microsoft\Windows\Start Menu\Programs\Startup\%scriptName%"

if exist "%target%" (
    del "%target%"
)