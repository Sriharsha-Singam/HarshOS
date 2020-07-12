FROM sriharshasingam/ubuntu-i386-elf-gcc-cross-compiler:latest
VOLUME . /src/HarshOS
WORKDIR /src/HarshOS
RUN make os.iso
RUN make kernel.elf