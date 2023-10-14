#include <Target/x64/x64Assembler.h>
#include "x64Assembler.h"

namespace air::x64
{
#define _dbgasm(fmt, ...) Info(fmt, __VA_ARGS__)

    void air::x64::Binary::Movb(Register base, int8_t disp, uint8_t imm8)
    {
        Emiter(0xC6); // 操作码
        uint8_t mod = (uint8_t)Mode::indirect_displaced << 6 | base.LowBit();
        Emiter(mod);
        Emiter(disp);
        Emiter(imm8);
        _dbgasm(" movb 0x%x -> [ %s + %d ]\n", imm8, base.mRegName, disp);
    }
    void Binary::Movw(Register base, int8_t disp, uint16_t imm16)
    {
        Emiter(0x66); // 操作数大写重载
        Emiter(0xC7); // 操作码
        uint8_t mod = (uint8_t)Mode::indirect_displaced << 6 | base.LowBit();
        Emiter(mod);
        Emiter(disp);
        Emiter2(imm16);
        _dbgasm(" movw 0x%x -> [ %s + %d ]\n", imm16, base.mRegName, disp);
    }
    void Binary::Movl(Register base, int8_t disp, uint32_t imm32)
    {
        Emiter(0xC7); // 操作码
        uint8_t mod = (uint8_t)Mode::indirect_displaced << 6 | base.LowBit();
        Emiter(mod);
        Emiter(disp);
        Emiter4(imm32);
        _dbgasm(" movl 0x%x -> [ %s + %d ]\n", imm32, base.mRegName, disp);
    }
    void Binary::Movq(Register base, int8_t disp, uint64_t imm64)
    {
        uint8_t rex = 0x48 | base.HightBit(); // rex前缀
        Emiter(rex);
        Emiter(0xC7); // 操作码
        uint8_t mod = (uint8_t)Mode::indirect_displaced << 6 | base.LowBit();
        Emiter(mod);
        Emiter(disp);
        Emiter8(imm64);
        _dbgasm(" movq 0x%llx -> [ %s + %d ]\n", imm64, base.mRegName, disp);
    }
}