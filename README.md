## NGCRecomp
NGCRecomp is a static recompilation tool for GameCube binaries. The goal of this project is to lift GameCube executable code (.dol and .rel) into a recompilable format using the Capstone disassembly engine.

Currently, the project is in the Experimental Phase, focusing on ISO parsing, DOL header validation, and Entry Point detection.

Project Structure:

> src/: Core logic for the recompiler.

''boot.c'': Entry point for the tool; handles ISO/DOL loading.

''external/capstone/'': The Capstone Disassembly Engine (v5.0.1) source code.

''gamerom.iso'': (Not tracked) The base GameCube ROM for analysis.

Current Progress
[x] ISO Disk Header Parsing

[x] DOL Header Extraction

[x] Big-Endian to Little-Endian Conversion

[x] Entry Point Detection (Target: The Wind Waker USA 1.00)

[ ] Section-to-Address Mapping

[ ] Instruction Lifting via Capstone

## Building
This project uses CMake 3.12+ and a C compiler (GCC/Clang).

Bash
mkdir build
cd build
cmake ..
cmake --build .
Usage
Bash
./NGCRecomp <path_to_game.iso>
Credits & Licensing
NGCRecomp: Licensed under the MIT License.

Capstone Engine: This project includes the Capstone disassembly framework, which is licensed under the BSD 3-Clause License.
