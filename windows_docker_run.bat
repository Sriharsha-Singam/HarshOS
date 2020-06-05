docker exec  -t -w /src/HarshOS ubuntu-cross-compiler make clean
start cmd.exe /c "docker exec  -it -w /src/HarshOS ubuntu-cross-compiler make run-curses"