#ifndef __CHARSTREAM__H__
#define __CHARSTREAM__H__

#include <Utils/Pch.h>

namespace air
{
    // 字符流
    struct CharStream
    {
        std::string data;
        uint32_t pos = 0;
        uint32_t line = 0;
        uint32_t col = 0;

        CharStream()
            : pos(0), line(1), col(1)
        {
        }

        // 设置字符缓存
        void SetData(const std::string &data)
        {
            this->data = data;
            pos = 0, line = 1, col = 1;
        }

        // 匹配一个字符
        bool Match(char ch, int off)
        {
            if (pos + off > data.size())
                return false;
            return data[pos + off] == ch;
        }
        // 查看一个字符
        char Peek() { return data[pos]; }
        char Peek(int offset) { return pos + offset > data.size() ? 0 : data[pos + offset]; }
        // 获取下一个字符
        char Next()
        {
            auto ch = data[pos++];
            if (ch == '\n')
            {
                line++;
                col = 0;
            }
            else
            {
                ++col;
            }
            return ch;
        }
        // 是否读取结束
        bool Eof() { return pos >= data.size() || data[pos] == '\0' ? true : false; }

        // 当前字符是否是字母和下划线
        bool IsChar_()
        {
            const char nChar = data[pos];
            if (('a' <= nChar && nChar <= 'z') ||
                ('A' <= nChar && nChar <= 'Z') ||
                nChar == '_')
                return true;
            return false;
        }
        // 当前字符是否是标志符元素
        bool IsIdent()
        {
            const char nChar = data[pos];
            if (('a' <= nChar && nChar <= 'z') ||
                ('A' <= nChar && nChar <= 'Z') ||
                ('0' <= nChar && nChar <= '9') ||
                nChar == '_')
                return true;
            return false;
        }
        // 是否是数字
        bool IsDigital()
        {
            const char nChar = data[pos];
            if ('0' <= nChar && nChar <= '9')
                return true;
            return false;
        }
        // 16进制字符
        bool IsHex()
        {
            const char nChar = data[pos];
            if (('0' <= nChar && nChar <= '9') ||
                ('A' <= nChar && nChar <= 'F') ||
                ('a' <= nChar && nChar <= 'f'))
                return true;
            return false;
        }
        // 8进制字符
        bool IsOct()
        {
            const char nChar = data[pos];
            if (('0' <= nChar && nChar <= '7'))
                return true;
            return false;
        }
        // 2进制字符
        bool IsBin()
        {
            const char nChar = data[pos];
            if (('0' == nChar) ||
                (nChar == '1'))
                return true;
            return false;
        }
        // 空白字符
        bool IsWhiteSpace()
        {
            const char nChar = data[pos];
            if (nChar == ' ' ||
                nChar == '\t' ||
                nChar == '\r' ||
                nChar == '\n')
                return true;
            return false;
        }
    };

}

#endif //!__CHARSTREAM__H__