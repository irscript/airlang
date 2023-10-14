#ifndef __X64OPERAND__H__
#define __X64OPERAND__H__
#include <Utils/Pch.h>
namespace air
{

    namespace x64
    {
        // 条件测试字段编码
        enum class Condition : uint8_t
        {
            overflow = 0,
            no_overflow = 1,
            below = 2,           // not above or equal
            not_below = 3,       // above or equal
            equal = 4,           // zero
            not_equal = 5,       // not zero
            below_equal = 6,     // not above
            not_below_equal = 7, // above
            sign = 8,
            not_sign = 9,
            parity = 10,               // parity even
            not_parity = 11,           // parity odd
            leass_than = 12,           // not greater than or equal
            not_leass_than = 13,       // greater than or equal
            leass_than_equal = 14,     // not greater than
            not_leass_than_equal = 15, // greater than

            // 别名
            carry = below,         // 进位
            not_carry = not_below, // 没有进位
            zero = equal,          // 为0
            not_zero = not_equal,  // 不为0
            negative = sign,       // 负数
            positive = not_sign,   // 正数

        };
        // 模式编码
        enum class Mode : uint8_t
        {
            indirect = 0,
            indirect_byte_displaced = 1,
            indirect_displaced = 2,
            direct = 3,
        };
        // 比例因子编码
        enum class ScaleFactor : uint8_t
        {
            times_1 = 0,
            times_2 = 1,
            times_4 = 2,
            times_8 = 3,
        };
        // 操作数

    }
}
#endif //!__X64OPERAND__H__