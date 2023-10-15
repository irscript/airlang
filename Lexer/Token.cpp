#include <Lexer/Token.h>
#include "Token.h"
#include <Utils/Loger.h>
namespace air
{
    std::string Token::ToString()
    {
        static const char *arr[] = {"错误单词", "宏关键字", "单行注释", "多行注释",
                                    "关键字", "标识符", "字符串常量", "字符常量",
                                    "整形常量", "浮点常量", "分隔符", "操作符",
                                    "文件结束", "行结束"};
        std::string szBuf;
        if (kind < TkKind::Max)
            szBuf.append(arr[(size_t)kind]);
        szBuf += " >text: " + text + pos.ToString();
        return szBuf;
    }

    void Token::GetCharVal(uint32_t &val)
    {
        int len = 0;
        val = 0;
        while (len < text.size())
        {
            uint8_t nChar = text[len];
            if (nChar == '\'')
                break;
            val = (val << 8) | nChar;
            ++len;
        }
        if (len > 4)
        {
            Waring("可能异常的 utf-8 字符：'%s' !\n", text.c_str());
        }
    }
    bool Token::GetIntVal(uint64_t &val, StringPool &pool, StringRef &type)
    {
        if (text.size() > 2 && text[0] == '0')
        {
            switch (text[1])
            {
            case 'B':
            case 'b':
            {
                val = strtoull(text.c_str(), nullptr, 16);
                type = val <= UINT32_MAX ? pool.RefString("uint32") : pool.RefString("uint64");
                return false;
            }
            break;
            case 'X':
            case 'x':
            {
                val = strtoull(text.c_str(), nullptr, 2);
                type = val <= UINT32_MAX ? pool.RefString("uint32") : pool.RefString("uint64");
                return false;
            }
            break;
            }
        }
        val = strtoull(text.c_str(), nullptr, 10);
        // 需要根据大小判断
        if (val <= INT32_MAX && val >= INT32_MIN)
        {
            type = pool.RefString("int32");
            return true;
        }
        else if (val <= UINT32_MAX && val >= 0)
        {
            type = pool.RefString("uint32");
            return false;
        }
        else if (val <= INT64_MAX && val >= INT64_MIN)
        {
            type = pool.RefString("int64");
            return true;
        }
        type = pool.RefString("uint64");
        return false;
    }
    void Token::GetFltVal(flt64_t &val, StringPool &pool, StringRef &type)
    {
        val = strtod(text.c_str(), nullptr);
        type = pool.RefString("flt64");
    }
}