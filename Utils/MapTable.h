#ifndef __MAPTABLE__H__
#define __MAPTABLE__H__

#include <Lexer/TkEnum.h>
#include <Utils/Pch.h>

namespace air
{
    struct MapTable
    {
        // 初始化
        static inline void Init()
        {
            InitKeyMap();
            InitMacroMap();
            InitOpPriorityMap();
        }

        // 查找关键字映射
        static KeyEnum FindKeyEnum(const std::string &szTok)
        {
            auto find = KeyMap.find(szTok);
            if (find == KeyMap.end())
                return KeyEnum::Unknown;
            return find->second;
        }
        // 查找宏关键字映射
        static MacroEnum FindMacroEnum(const std::string &szTok)
        {
            auto find = MacroMap.find(szTok);
            if (find == MacroMap.end())
                return MacroEnum::Unknown;
            return find->second;
        }
        // 查找操作符优先级
        static int32_t FindOpPriority(OpEnum op)
        {
            auto find = OpPriorityMap.find(op);
            if (find == OpPriorityMap.end())
                return -1;
            return find->second;
        }

    private:
        // 关键字与枚举映射
        static std::map<std::string, KeyEnum> KeyMap;     // 关键字
        static std::map<std::string, MacroEnum> MacroMap; // 宏关键字
        static std::map<OpEnum, int32_t> OpPriorityMap;   // 操作符优先级映射

        static void InitKeyMap();
        static void InitMacroMap();
        static void InitOpPriorityMap();
    };

}

#endif //!__MAPTABLE__H__