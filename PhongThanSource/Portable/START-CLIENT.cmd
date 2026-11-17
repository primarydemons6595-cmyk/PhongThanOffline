@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Start-Portable.ps1" -Action Client
if errorlevel 1 pause
