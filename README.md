 ## NGCRecomp
NGCRecomp is a static recompilation tool for GameCube binaries. The goal of this project is to lift GameCube executable code (.dol and .rel) into a recompilable format using the Capstone disassembly engine.

Currently, the project is in the Experimental Phase, focusing on ISO parsing, DOL header validation, and Entry Point detection.

Project Structure:

`src/`: Core logic for the recompiler.

`boot.c`: Entry point for the tool; handles ISO/DOL loading.

`external/capstone/`: The Capstone Disassembly Engine (v5.0.1) source code.

`gamerom.iso`: (Not tracked) The base GameCube ROM for analysis.

Current Progress
- [x] ISO Disk Header Parsing

- [x] DOL Header Extraction

- [x] Big-Endian to Little-Endian Conversion

- [x] Entry Point Detection (Target: The Wind Waker USA 1.00)

- [ ] Section-to-Address Mapping

- [ ] Instruction Lifting via Capstone

# Notice
NGCRecomp is not affiliated with Nintendo in any way. NGCRecomp does not contain any copyrighted assets. You must provide your own .ISO
## Building
This project uses CMake 3.12+ and a C compiler (GCC/Clang).

The first step is to build Capstone. From your repo root folder. Enter
```
cd external/capstone
```
After that. Enter the following commands in this exact order
```
mkdir build
cd build
cmake ..
cmake --build .
```
Once it has finished compiling. Go back to the root folder. And repeat the same build process for building Capstone.
## Usage
```
./NGCRecomp <path_to_game.iso>
```
# Limitations
- Inside the recompiled C code of the game. You will notice some unhandled instructions. This is because not all instructions have been added to decode. But many of them have already been implemented.
- This tool is (obviously) still experimental and is not finished.
## Credits & Licensing
- NGCRecomp: Licensed under the MIT License.
- Capstone Engine: This project includes the Capstone disassembly framework, which is licensed under the BSD 3-Clause License.
