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

    FILE *file = fopen(argv[1], "rb");
    if (!file) return 2;

    GCMHeader gcm;
    fread(&gcm, sizeof(GCMHeader), 1, file);

    // Swap big-endian offset for Windows
    uint32_t dolPos = _byteswap_ulong(gcm.dolOffset);

    fseek(file, dolPos, SEEK_SET);
    DOLHeader dol;
    fread(&dol, sizeof(DOLHeader), 1, file);

    printf("Entry Point: 0x%X\n", _byteswap_ulong(dol.entryPoint));

    uint32_t target = _byteswap_ulong(dol.entryPoint);

    for (int i = 0; i < 7; i++) {
        uint32_t start = _byteswap_ulong(dol.textAddress[i]);
        uint32_t size = _byteswap_ulong(dol.textSize[i]);
        uint32_t fileOff = _byteswap_ulong(dol.textOffset[i]);
        if (target >= start && target < (start + size)) {
            uint32_t finalPos = dolPos + fileOff + (target - start);
            fseek(file, finalPos, SEEK_SET);
            uint8_t buffer[16];
            fread(buffer, 1, 16, file);
            csh handle;
            cs_open(CS_ARCH_PPC, CS_MODE_32, &handle);
            cs_insn *insn;
            size_t count = cs_disasm(handle, buffer, 16, target, 0, &insn);
            if (count > 0) {
                for (size_t j = 0; j < count; j++) {
                    printf("0x%X:\t%s\t%s\n", insn[j].address, insn[j].mnemonic, insn[j].op_str);
                }
                cs_free(insn, count);
            } else {
                printf("Failed to disassemble at 0x%X\n", target);
            }
            break;
        }
    }
    fclose(file);
    return 0;
}