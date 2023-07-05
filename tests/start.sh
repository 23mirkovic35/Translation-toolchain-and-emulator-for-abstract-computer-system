g++ ./src/MainAsm.cpp ./src/asembler.cpp ./src/RelokacioniZapis.cpp ./src/Sekcija.cpp ./src/Simbol.cpp ./src/Tabele.cpp -o assembler
g++ ./src/MainLinker.cpp ./src/linker.cpp ./src/SekcijaLinker.cpp ./src/SimbolLinker.cpp ./src/RelokacioniZapisLinker.cpp -o linker
g++ ./src/MainEmulator.cpp ./src/emulator.cpp -o emulator

./emulator ./tests/program.hex