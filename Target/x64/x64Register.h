#ifndef __X64REGISTER__H__
#define __X64REGISTER__H__
#include <Utils/Pch.h>
namespace air
{
    namespace x64
    {
        //---------------定义寄存器类型-------------
        // x64 寄存器 基类
        struct IRegister
        {
            uint8_t mRegCode;     // 寄存器编码
            const char *mRegName; // 寄存器名称
            // 编码是否有效
            constexpr bool IsValid() const { return mRegCode != -1; }
            // 生成REX指令前缀时使用，返回 第4位的值 ：0 或者 1
            constexpr uint8_t HightBit() const { return mRegCode >> 3; }
            // 生成 modR/M, SIB, 操作数字节时使用.返回 低 3 位的值
            constexpr uint8_t LowBit() const { return mRegCode & 0x7; }

        protected:
            explicit IRegister(uint8_t code, const char *name) : mRegCode(code), mRegName(name) {}
        };

        // x64 通用寄存器: 64 bit
        struct Register : public IRegister
        {
            explicit Register(uint8_t code, const char *name) : IRegister(code, name) {}
            // 获取最后一个寄存器编码
            constexpr static uint8_t GetLastCode() { return r15; }
            constexpr bool IsValid() const { return mRegCode < regMax; }
            constexpr bool operator<(const Register &rhs) { return mRegCode < rhs.mRegCode; }
            // 通用寄存器编码
            enum : uint8_t
            {
                rax,
                rcx,
                rdx,
                rbx,
                rsp,
                rbp,
                rsi,
                rdi,
                r8,
                r9,
                r10,
                r11,
                r12,
                r13,
                r14,
                r15,
                regMax
            };
        };
        // x64 XMM 寄存器: 128 bit
        struct XMMRegister : public IRegister
        {
            explicit XMMRegister(uint8_t code, const char *name) : IRegister(code, name) {}
            // 获取最后一个寄存器编码
            constexpr static uint32_t GetLastCode() { return xmm15; }
            constexpr bool operator<(const XMMRegister &rhs) { return mRegCode < rhs.mRegCode; }
            // 通用寄存器编码
            enum : uint8_t
            {
                xmm0,
                xmm1,
                xmm2,
                xmm3,
                xmm4,
                xmm5,
                xmm6,
                xmm7,
                xmm8,
                xmm9,
                xmm10,
                xmm11,
                xmm12,
                xmm13,
                xmm14,
                xmm15,
                regMax
            };
        };
        // x64 YMM 寄存器:256 bit
        struct YMMRegister : public IRegister
        {
            explicit YMMRegister(uint8_t code, const char *name) : IRegister(code, name) {}
            // 获取最后一个寄存器编码
            constexpr static uint32_t GetLastCode() { return ymm15; }
            constexpr bool operator<(const YMMRegister &rhs) { return mRegCode < rhs.mRegCode; }
            // 通用寄存器编码
            enum : uint8_t
            {
                ymm0,
                ymm1,
                ymm2,
                ymm3,
                ymm4,
                ymm5,
                ymm6,
                ymm7,
                ymm8,
                ymm9,
                ymm10,
                ymm11,
                ymm12,
                ymm13,
                ymm14,
                ymm15,
                regMax
            };
        };

        //------------------定义寄存器集合----------------
        extern const Register gGReg[Register::regMax];         // 64 位通用寄存器集合
        extern const XMMRegister gXMMReg[XMMRegister::regMax]; // 128 位浮点寄存器集合
        extern const YMMRegister gYMMReg[YMMRegister::regMax]; // 256 位浮点寄存器集合
        //------------------可分配寄存器集合---------------

    }
}

#endif //!__X64REGISTER__H__