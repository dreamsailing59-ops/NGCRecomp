 ## NGCRecomp
NGCRecomp is a static recompilation tool for GameCube binaries. The goal of this project is to lift GameCube executable code (.dol and .rel) into a recompilable format using the Capstone disassembly engine.

Currently, the project is in the Experimental Phase, focusing on ISO parsing, DOL header validation, and Entry Point detection.

Project Structure:

`src/`: Core logic for the recompiler.

`boot.c`: Entry point for the tool; handles ISO/DOL loading.

`external/capstone/`: The Capstone Disassembly Engine (v5.0.1) source code.

`gamerom.iso`: (Not tracked) The base GameCube ROM for analysis.

Current Progress
- [x] ISO Disk Header Parsing: Successfully reading the GCM/GCE format.

- [x] DOL Header Extraction: Parsing text/data section offsets and sizes.

- [x] Big-Endian to Little-Endian Conversion: Manual byte-swapping for Windows/UCRT64 compatibility.

- [x] Entry Point Detection: Targeted at The Wind Waker (USA 1.00) at 0x80003140.

- [x] Section-to-Address Mapping: Translation loop implemented to map RAM addresses to ISO file offsets.

- [x] Instruction Lifting via Capstone:

- [x] Basic Arithmetic (addi, add, li, lis).

- [x] Load/Store Logic (lwz, stw, stwu, stb).

- [x] Logical Operations (or, mr, rlwinm, andi.).

- [x] Control Flow Translation:

- [x] Function Call Lifting (bl to C function calls).

- [x] Conditional Branching (bt, bc to C if/goto).

- [x] Unconditional Jumps (b to C goto).

- [ ] Static Block Recovery: (Next Step) Automatically tracing and disassembling full functions until blr.

- [ ] Global Pointer / TOC Handling: Mapping r2 and r13 for static data access.

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
