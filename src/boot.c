#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <capstone/capstone.h>

#pragma pack(push, 1)

typedef struct {
    char     consoleID;       // 0x000
    char     gameCode[2];     // 0x001
    char     regionCode;      // 0x003
    char     makerCode[2];    // 0x004
    uint8_t  discID;          // 0x006
    uint8_t  version;         // 0x007
    uint8_t  audioStreaming;  // 0x008
    uint8_t  streamBufSize;   // 0x009
    uint8_t  unused1[18];     // 0x00A
    uint32_t magic;           // 0x01C
    uint8_t  unused2[1024];   // 0x020
    uint32_t dolOffset;       // 0x420
    uint32_t fstOffset;       // 0x424
    uint32_t fstSize;         // 0x428
    uint32_t fstMaxSize;      // 0x42C
} GCMHeader;

typedef struct {
    uint32_t textOffset[7];   // 0x000
    uint32_t dataOffset[11];  // 0x01C
    uint32_t textAddress[7];  // 0x048
    uint32_t dataAddress[11]; // 0x064
    uint32_t textSize[7];     // 0x090
    uint32_t dataSize[11];    // 0x0AC
    uint32_t bssAddress;      // 0x0D8
    uint32_t bssSize;         // 0x0DC
    uint32_t entryPoint;      // 0x0E0
    uint8_t  unused[28];      // 0x0E4
} DOLHeader;

#pragma pack(pop)

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;

    FILE *ISO = fopen(argv[1], "rb");
    if (ISO == NULL) {
        perror("Failed to open ISO");
        return 1;
    }

    GCMHeader gcm;
    fread(&gcm, sizeof(GCMHeader), 1, ISO);

    // Swap big-endian offset for Windows
    uint32_t dolPos = _byteswap_ulong(gcm.dolOffset);

    fseek(ISO, dolPos, SEEK_SET);
    DOLHeader dol;
    fread(&dol, sizeof(DOLHeader), 1, ISO);

    printf("Entry Point: 0x%X\n", _byteswap_ulong(dol.entryPoint));

    uint32_t target = _byteswap_ulong(dol.entryPoint);

    FILE *out = fopen("recompiled_game.c", "w");
    fprintf(out, "#include \"cpu.h\"\n\n");
    fprintf(out, "void game_start(GekkoCPU *cpu) {\n");

    for (int i = 0; i < 7; i++) {
        uint32_t start = _byteswap_ulong(dol.textAddress[i]);
        uint32_t size = _byteswap_ulong(dol.textSize[i]);
        uint32_t ISOOff = _byteswap_ulong(dol.textOffset[i]);
        if (target >= start && target < (start + size)) {
            uint32_t finalPos = dolPos + ISOOff + (target - start);
            fseek(ISO, finalPos, SEEK_SET);
            uint8_t buffer[16];
            fread(buffer, 1, 16, ISO);
            csh handle;
            cs_open(CS_ARCH_PPC, CS_MODE_BIG_ENDIAN, &handle);
            cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);
            cs_insn *insn;
            size_t count = cs_disasm(handle, buffer, 16, target, 0, &insn);
            if (count > 0) {
                for (size_t j = 0; j < count; j++) {
                    printf("0x%X:\t%s\t%s\n", (unsigned)insn[j].address, insn[j].mnemonic, insn[j].op_str);

                    if (!insn[j].detail) {
                        fprintf(out, "    // Instruction at 0x%X: %s %s (no detail)\n", (unsigned)insn[j].address, insn[j].mnemonic, insn[j].op_str);
                        continue;
                    }

                    switch (insn[j].id) {
                        case PPC_INS_LI: {
                            int reg_d = insn[j].detail->ppc.operands[0].reg - PPC_REG_R0;
                            // CRITICAL: Ensure we grab the IMM value, not the REG value
                            int32_t imm = (int32_t)insn[j].detail->ppc.operands[1].imm;
                            fprintf(out, "    cpu->gpr[%d] = %d;\n", reg_d, imm);
                            break;
                        }

                        case PPC_INS_ADDI: {
                            int reg_d = insn[j].detail->ppc.operands[0].reg - PPC_REG_R0;
                            
                            // If the second operand is a register, do addition
                            if (insn[j].detail->ppc.operands[1].type == PPC_OP_REG) {
                                // Special Case: r0 in an addi instruction IS the value 0
                                if (insn[j].detail->ppc.operands[1].reg == PPC_REG_R0) {
                                    int32_t imm = (int32_t)insn[j].detail->ppc.operands[2].imm;
                                    fprintf(out, "    cpu->gpr[%d] = %d;\n", reg_d, imm);
                                } else {
                                    int reg_a = insn[j].detail->ppc.operands[1].reg - PPC_REG_R0;
                                    int32_t imm = (int32_t)insn[j].detail->ppc.operands[2].imm;
                                    fprintf(out, "    cpu->gpr[%d] = cpu->gpr[%d] + %d;\n", reg_d, reg_a, imm);
                                }
                            } 
                            break;
                        }

                        case PPC_INS_BL: {
                            // bl target => call a C function at that address
                            uint32_t target_addr = (uint32_t)insn[j].detail->ppc.operands[0].imm;
                            fprintf(out, "    func_0x%X(cpu);\n", target_addr);
                            break;
                        }

                        case PPC_INS_STWU: {
                            int reg_s = insn[j].detail->ppc.operands[0].reg - PPC_REG_R0;
                            int reg_a = insn[j].detail->ppc.operands[1].mem.base - PPC_REG_R0;
                            int32_t disp = (int32_t)insn[j].detail->ppc.operands[1].mem.disp;

                            // We MUST save the value BEFORE we update the register if we want to be accurate
                            fprintf(out, "    Write32(cpu, cpu->gpr[%d] + %d, cpu->gpr[%d]);\n", reg_a, disp, reg_s);
                            fprintf(out, "    cpu->gpr[%d] += %d;\n", reg_a, disp);
                            break;
                        }

                        case PPC_INS_LWZ: {
                            int reg_d = insn[j].detail->ppc.operands[0].reg - PPC_REG_R0;
                            int reg_a = insn[j].detail->ppc.operands[1].mem.base - PPC_REG_R0;
                            int32_t disp = (int32_t)insn[j].detail->ppc.operands[1].mem.disp;

                            fprintf(out, "    cpu->gpr[%d] = Read32(cpu, cpu->gpr[%d] + %d);\n", reg_d, reg_a, disp);
                            break;
                        }
                        
                        default: {
                            fprintf(out, "    // Unhandled instruction: %s %s\n", insn[j].mnemonic, insn[j].op_str);
                            break;
                        }
                    }
                }
                cs_free(insn, count);
            } else {
                printf("Failed to disassemble at 0x%X\n", target);
            }
            cs_close(&handle);
        }
    }

    fprintf(out, "}\n");
    fclose(out);

    fclose(ISO);
    printf("Recompilation complete! Output written to recompiled_game.c\n");
    return 0;
}