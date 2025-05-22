@echo off
setlocal enabledelayedexpansion

(for %%f in (*.h) do (
    echo ==== %%f ====
    type "%%f"
    echo.
    echo.
)) > h_fajlok.txt
