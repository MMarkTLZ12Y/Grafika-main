@echo off
setlocal enabledelayedexpansion

(for %%f in (*.c) do (
    echo ==== %%f ====
    type "%%f"
    echo.
    echo.
)) > c_fajlok.txt
