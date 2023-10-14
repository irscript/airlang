#ifndef __TOKEN__H__
#define __TOKEN__H__
#include <Lexer/TkEnum.h>
#include <Utils/Pch.h>
#include <Utils/StringPool.h>
#include <Utils/ErrorExp.h>
namespace air
{
    // 单词所在位置
    struct TkPos
    {
        uint32_t mOffset; // 所在文件的偏移
        uint32_t mLine;   // 所在的行

        TkPos() : mOffset(0), mLine(0) {}
        TkPos(uint32_t offset, uint32_t line)
            : mOffset(offset), mLine(line) {}
        std::string ToString()
        {
            std::stringstream ss;

            ss << " Pos[ ";
            ss << mOffset;
            ss << ", ";
            ss << mLine;
            ss << " ]";
            return ss.str();
        }
    };
    // 单词定义
    struct Token
    {
        std::string text; // 单词文本
        TkPos pos;        // 单词所在位置
        TkKind kind;      // 单词类别
        union
        {
            SpEnum sp;     // 分隔符
            OpEnum op;     // 操作符
            KeyEnum key;   // 关键字
            MacroEnum mac; // 宏关键字
        } code;

        Token() : kind(TkKind::Error) {}
        Token(const Token &rhs)
        {
            text = rhs.text;
            pos = rhs.pos;
            kind = rhs.kind;
            code.op = rhs.code.op;
        }
        Token(TkKind kind, TkPos &pos)
            : kind(kind), pos(pos) {}
        Token(TkKind kind, char ch, TkPos &pos)
            : kind(kind), pos(pos) { text.push_back(ch); }
        Token(TkKind kind, char ch, TkPos &pos, SpEnum sp)
            : kind(kind), pos(pos)
        {
            text.push_back(ch);
            code.sp = sp;
        }
        Token(TkKind kind, char ch, TkPos &pos, OpEnum op)
            : kind(kind), pos(pos)
        {
            text.push_back(ch);
            code.op = op;
        }
        Token(TkKind kind, char ch, TkPos &pos, KeyEnum key)
            : kind(kind), pos(pos)
        {
            text.push_back(ch);
            code.key = key;
        }

        Token(TkKind kind, const std::string &txt, TkPos &pos)
            : kind(kind), text(txt), pos(pos) {}
        Token(TkKind kind, const std::string &txt, TkPos &pos, SpEnum sp)
            : kind(kind), text(txt), pos(pos) { code.sp = sp; }
        Token(TkKind kind, const std::string &txt, TkPos &pos, OpEnum op)
            : kind(kind), text(txt), pos(pos) { code.op = op; }
        Token(TkKind kind, const std::string &txt, TkPos &pos, KeyEnum key)
            : kind(kind), text(txt), pos(pos) { code.key = key; }
        Token(TkKind kind, const std::string &txt, TkPos &pos, MacroEnum mac)
            : kind(kind), text(txt), pos(pos) { code.mac = mac; }

        std::string ToString();

        // 获取字符常量的值
        void GetCharVal(uint32_t &val);
        // 获取整数常量的值:true,有符号整数；false，无符号整数
        bool GetIntVal(uint64_t &val, StringRef &type);
        // 获取浮点常量的值
        void GetFltVal(flt64_t &val, StringRef &type);
    };

}

#endif //!__TOKEN__H__