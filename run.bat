@echo off
$OutputEncoding = [Console]::OutputEncoding = [Text.UTF8Encoding]::new()
gcc tarea3.c tdas\list.c tdas\extra.c tdas\hashmap.c -o tarea3
tarea3.exe

pause