docker exec -w /src/HarshOS ubuntu-cross-compiler make clean
docker exec -w /src/HarshOS ubuntu-cross-compiler make os.iso
start cmd.exe @cmd /k "C:\Program Files (x86)\qemu\qemu-system-i386.exe" -s -S -fda C:\DockerShared\HarshOS\build_os\os.iso