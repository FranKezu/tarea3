@echo off
chcp 65001
gcc tarea3.c tdas\list.c tdas\extra.c tdas\hashmap.c -o tarea3
tarea3.exe

pause