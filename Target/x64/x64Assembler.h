#ifndef __X64ASSEMBLER__H__
#define __X64ASSEMBLER__H__

#include <Utils/Pch.h>
#include <Target/x64/x64Register.h>
#include <Target/x64/x64Operand.h>
#include <Utils/Loger.h>
namespace air::x64
{

    // x64 机器代码生成器
    struct Binary
    {
        //--------通用函数------
        auto GetSize() { return mCode.size(); }
        auto GetCurrentPos() { return mCode.size(); }
        auto GetData() { return mCode.data(); }

        //---------助记符生成到机器码----------------

        // ret: 函数返回
        void Ret() { Emiter(0xC3); }
        // nop : 空操作，一般只为内存对齐而使用
        void Nop() { Emiter(0x90); }

        //------------数据传输指令-----------
        /** movb imm8 -> [base+disp]
         * @param base 内存基地址
         * @param disp 地址偏移
         * @param imm8 8位立即数
         */
        void Movb(Register base, int8_t disp, uint8_t imm8);
        /** movw imm16 -> [base+disp]
         * @param base 内存基地址
         * @param disp 地址偏移
         * @param imm16 16位立即数
         */
        void Movw(Register base, int8_t disp, uint16_t imm16);
        /** movl imm32 -> [base+disp]
         * @param base 内存基地址
         * @param disp 地址偏移
         * @param imm32 32位立即数
         */
        void Movl(Register base, int8_t disp, uint32_t imm32);
        /** movq imm64 -> [base+disp]
         * @param base 内存基地址
         * @param disp 地址偏移
         * @param imm64 32位立即数
         */
        void Movq(Register base, int8_t disp, uint64_t imm64);

        //-------------私有内部成员、函数----------------
    private:
        using Buffer = std::vector<uint8_t>;
        // 代码段
        Buffer mCode;

        // 写入1个字节
        void Emiter(uint8_t val) { mCode.push_back(val); }
        // 写入2个字节
        void Emiter2(uint16_t val)
        {
            mCode.push_back(val >> 8);
            mCode.push_back(val);
        }
        // 写入4个字节
        void Emiter4(uint32_t val)
        {
            mCode.push_back(val >> 24);
            mCode.push_back(val >> 16);
            mCode.push_back(val >> 8);
            mCode.push_back(val);
        }
        // 写入8个字节
        void Emiter8(uint64_t val)
        {
            mCode.push_back(val >> 56);
            mCode.push_back(val >> 48);
            mCode.push_back(val >> 40);
            mCode.push_back(val >> 32);
            mCode.push_back(val >> 24);
            mCode.push_back(val >> 16);
            mCode.push_back(val >> 8);
            mCode.push_back(val);
        }
    };
}

#endif //!__X64ASSEMBLER__H__