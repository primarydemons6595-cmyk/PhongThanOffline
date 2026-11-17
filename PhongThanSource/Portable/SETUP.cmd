@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Setup-Portable.ps1" -InstallPrerequisites %*
if errorlevel 1 (echo Setup failed. Read the error above. & pause & exit /b 1)
pause
