docker exec -w /src/HarshOS ubuntu-cross-compiler make clean
docker exec -w /src/HarshOS ubuntu-cross-compiler make os.iso
"C:\Program Files (x86)\qemu\qemu-system-i386.exe" -fda C:\DockerShared\HarshOS\build_os\os.iso
@rem start cmd.exe /c "docker exec  -it -w /src/HarshOS ubuntu-cross-compiler make run-curses"
@rem start cmd.exe /c "docker exec  -it -w /src/HarshOS ubuntu-cross-compiler make debug-curses"
@rem start cmd.exe /c "docker exec  -it -w /src/HarshOS ubuntu-cross-compiler /usr/local/i386elfgcc/bin/i386-elf-gdb -ex "target remote localhost:1234" -ex "symbol-file ./build_os/kernel.elf""