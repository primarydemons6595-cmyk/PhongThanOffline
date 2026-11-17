@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Start-Portable.ps1" -Action Studio
if errorlevel 1 pause
