@echo off
set "source=..\..\Host\src\scripts\run_exec.bat"
set "target=%appdata%\Microsoft\Windows\Start Menu\Programs\Startup\"

copy "%source%" "%target%"
pause