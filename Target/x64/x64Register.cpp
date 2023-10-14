#include <Target/x64/x64Register.h>
namespace air
{
    namespace x64
    { // 64 位通用寄存器集合
        const Register gGReg[] = {
            Register(Register::rax, "rax"),
            Register(Register::rcx, "rcx"),
            Register(Register::rdx, "rdx"),
            Register(Register::rbx, "rbx"),
            Register(Register::rsp, "rsp"),
            Register(Register::rbp, "rbp"),
            Register(Register::rsi, "rsi"),
            Register(Register::rdi, "rdi"),
            Register(Register::r8, "r8"),
            Register(Register::r9, "r9"),
            Register(Register::r10, "r10"),
            Register(Register::r11, "r11"),
            Register(Register::r12, "r12"),
            Register(Register::r13, "r13"),
            Register(Register::r14, "r14"),
            Register(Register::r15, "r15"),
        };
        // 128 位浮点寄存器集合
        const XMMRegister gXMMReg[] = {
            XMMRegister(XMMRegister::xmm0, "xmm0"),
            XMMRegister(XMMRegister::xmm1, "xmm1"),
            XMMRegister(XMMRegister::xmm2, "xmm2"),
            XMMRegister(XMMRegister::xmm3, "xmm3"),
            XMMRegister(XMMRegister::xmm4, "xmm4"),
            XMMRegister(XMMRegister::xmm5, "xmm5"),
            XMMRegister(XMMRegister::xmm6, "xmm6"),
            XMMRegister(XMMRegister::xmm7, "xmm7"),
            XMMRegister(XMMRegister::xmm8, "xmm8"),
            XMMRegister(XMMRegister::xmm9, "xmm9"),
            XMMRegister(XMMRegister::xmm10, "xmm10"),
            XMMRegister(XMMRegister::xmm11, "xmm11"),
            XMMRegister(XMMRegister::xmm12, "xmm12"),
            XMMRegister(XMMRegister::xmm13, "xmm13"),
            XMMRegister(XMMRegister::xmm14, "xmm14"),
            XMMRegister(XMMRegister::xmm15, "xmm15"),

        };
        // 256 位浮点寄存器集合
        const YMMRegister gYMMReg[] = {
            YMMRegister(YMMRegister::ymm0, "ymm0"),
            YMMRegister(YMMRegister::ymm1, "ymm1"),
            YMMRegister(YMMRegister::ymm2, "ymm2"),
            YMMRegister(YMMRegister::ymm3, "ymm3"),
            YMMRegister(YMMRegister::ymm4, "ymm4"),
            YMMRegister(YMMRegister::ymm5, "ymm5"),
            YMMRegister(YMMRegister::ymm6, "ymm6"),
            YMMRegister(YMMRegister::ymm7, "ymm7"),
            YMMRegister(YMMRegister::ymm8, "ymm8"),
            YMMRegister(YMMRegister::ymm9, "ymm9"),
            YMMRegister(YMMRegister::ymm10, "ymm10"),
            YMMRegister(YMMRegister::ymm11, "ymm11"),
            YMMRegister(YMMRegister::ymm12, "ymm12"),
            YMMRegister(YMMRegister::ymm13, "ymm13"),
            YMMRegister(YMMRegister::ymm14, "ymm14"),
            YMMRegister(YMMRegister::ymm15, "ymm15"),

        };
    }
}