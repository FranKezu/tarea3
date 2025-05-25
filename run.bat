@echo off
chcp 65001
gcc main.c tdas\list.c tdas\extra.c tdas\hashmap.c grafo.c -o main -lwinmm

main.exe

pause