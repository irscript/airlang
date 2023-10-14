#include <Lexer/Token.h>
#include "Token.h"

namespace air
{
    std::string Token::ToString()
    {
        static const char *arr[] = {"错误单词", "宏关键字", "单行注释","多行注释",
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
    }
    bool Token::GetIntVal(uint64_t &val, StringRef &type)
    {
        return false;
    }
    void Token::GetFltVal(flt64_t &val, StringRef &type)
    {
    }
}