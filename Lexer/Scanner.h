#ifndef __SCANNER__H__
#define __SCANNER__H__

#include <Lexer/Token.h>
#include <Lexer/CharStream.h>
#include <Utils/Loger.h>

namespace air
{
    // 词法扫描器
    class Scanner
    {
    private:
        // 预存退回token
        std::list<Token> mBackTok;
        CharStream mStream; // 字符输入流
        TkPos mLastPos;     // 记录最后一个获取的token的位置
    public:
        // 获取流存储
        std::string &GetStream() { return mStream.data; }
        // 获取下一个单词,不包含注释
        Token GetNext();
        // 获取注释,没有注释则返回空字符串
        Token GetComment();
        TkPos GetCurPos()
        {
            return TkPos(mStream.pos, mStream.line);
        }

        // 退回单词:按照逆向顺序
        void BackTok(Token &tok)
        {
            // mBackTok.push_front(tok);
            mStream.pos = tok.pos.mOffset;
            mStream.line = tok.pos.mLine;
        }
        // 跳过空白字符
        inline void SkipWhiteSpaces()
        {
            while (mStream.IsWhiteSpace() == true)
                mStream.Next();
        }

    private:
        // 解析一个单词
        Token GetToken();
        // 解析操作符或者分隔符
        Token GetOperator();
        // 解析一个单行注释
        Token GetSinglineComment();
        // 解析多行注释
        Token GetMultilineComment();
        // 解析字符串常量
        Token GetString(bool escape);
        // 解析标识符
        Token GetIdent();
        // 解析宏关键字
        Token GetMacro();
        // 解析字符常量
        Token GetChar();
        // 解析整数或者浮点常量
        Token GetDigit();
        // 解析浮点数
        Token GetFltNumber(std::string &szBuf);
    };
}

#endif //!__SCANNER__H__