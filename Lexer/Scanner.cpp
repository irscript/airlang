#include "Scanner.h"
#include "Utils/MapTable.h"
namespace air
{
    Token Scanner::GetNext()
    {
        // if (mBackTok.empty() == true)
        {
            while (true)
            {
                auto tok = GetToken();
                if (tok.kind == TkKind::Notes || tok.kind == TkKind::MultiNotes)
                    continue;
                mLastPos = tok.pos;
                return tok;
            }
        }
        /*auto tok = mBackTok.front();
        mBackTok.pop_front();
        mLastPos = tok.pos;*/
        return Token();
    }
    Token Scanner::GetComment()
    {
        auto tok = GetToken();
        if (tok.kind == TkKind::Notes || tok.kind == TkKind::MultiNotes)
            return tok;
        BackTok(tok);
        return Token();
    }
    Token Scanner::GetToken()
    {
        // 跳过空白字符
        SkipWhiteSpaces();

        auto pos = GetCurPos();
        if (mStream.Eof() == true)
            return Token(TkKind::Eof, " EOF ", pos);

        // 解析标识符
        if (mStream.IsChar_() == true)
            return GetIdent();
        // 解析数字常量
        if (mStream.IsDigital() == true)
            return GetDigit();

        auto ch = mStream.Peek();
        switch (ch)
        {
            // 宏关键字
        case '@':
            return mStream.Peek(1) == '"' ? GetString(true) : GetMacro();
            // 字符常量
        case '\'':
            return GetChar();
            // 字符串
        case '"':
            return GetString(false);
        }
        return GetOperator();
    }
    Token Scanner::GetOperator()
    {
        auto ch = mStream.Peek();
        auto pos = GetCurPos();
        switch (ch)
        {
        //--------解析分隔符----------
        case '(':
        {
            mStream.Next();
            return Token(TkKind::Seperator, ch, pos, SpEnum::OpenParen);
        }
        break;
        case ')':
        {
            mStream.Next();
            return Token(TkKind::Seperator, ch, pos, SpEnum::CloseParen);
        }
        break;
        case '[':
        {
            mStream.Next();
            return Token(TkKind::Seperator, ch, pos, SpEnum::OpenBracket);
        }
        break;
        case ']':
        {
            mStream.Next();
            return Token(TkKind::Seperator, ch, pos, SpEnum::CloseBracket);
        }
        break;
        case '{':
        {
            mStream.Next();
            return Token(TkKind::Seperator, ch, pos, SpEnum::OpenBrace);
        }
        break;
        case '}':
        {
            mStream.Next();
            return Token(TkKind::Seperator, ch, pos, SpEnum::CloseBrace);
        }
        break;
        case ':':
        {
            mStream.pos += 1;
            return Token(TkKind::Seperator, ":", pos, SpEnum::Colon);
        }
        break;
        case ',':
        {
            mStream.Next();
            return Token(TkKind::Seperator, ch, pos, SpEnum::Comma);
        }
        break;
        case ';':
        {
            mStream.Next();
            return Token(TkKind::Seperator, ch, pos, SpEnum::SemiColon);
        }
        break;
        //--------解析操作符----------
        case '.':
        {
            if (mStream.Match(ch, 1) == true)
            {
                if (mStream.Match(ch, 2) == true)
                {
                    mStream.pos += 3;
                    return Token(TkKind::Seperator, "...", pos, SpEnum::Ellipsis);
                }
            }
            mStream.pos += 1;
            return Token(TkKind::Operaor, ch, pos, OpEnum::Dot);
        }
        break;
        case '?':
        {
            mStream.pos += 1;
            return Token(TkKind::Operaor, ch, pos, OpEnum::Question);
        }
        break;
        case '>':
        {
            // >>
            if (mStream.Match(ch, 1) == true)
            {
                // >>=
                if (mStream.Match('=', 2) == true)
                {
                    mStream.pos += 3;
                    return Token(TkKind::Operaor, ">>=", pos, OpEnum::LeftShiftAssign);
                }
                mStream.pos += 2;
                return Token(TkKind::Operaor, ">>", pos, OpEnum::LeftShift);
            } // >=
            if (mStream.Match('=', 1) == true)
            {
                mStream.pos += 2;
                return Token(TkKind::Operaor, ">=", pos, OpEnum::GE);
            }
            mStream.pos += 1;
            return Token(TkKind::Operaor, ch, pos, OpEnum::GT);
        }
        break;
        case '<':
        { // <<
            if (mStream.Match(ch, 1) == true)
            {
                // <<<
                if (mStream.Match(ch, 2) == true)
                {
                    // <<<=
                    if (mStream.Match('=', 3) == true)
                    {
                        mStream.pos += 4;
                        return Token(TkKind::Operaor, "<<<=", pos, OpEnum::LogicalRightShiftAssign);
                    }
                    mStream.pos += 3;
                    return Token(TkKind::Operaor, "<<<", pos, OpEnum::LogicalRightShift);
                } // <<=
                if (mStream.Match('=', 2) == true)
                {
                    mStream.pos += 3;
                    return Token(TkKind::Operaor, "<<=", pos, OpEnum::RightShiftAssign);
                }
                mStream.pos += 2;
                return Token(TkKind::Operaor, "<<", pos, OpEnum::RightShift);
            }
            // <=
            if (mStream.Match('=', 1) == true)
            {
                mStream.pos += 2;
                return Token(TkKind::Operaor, "<=", pos, OpEnum::LE);
            }
            mStream.pos += 1;
            return Token(TkKind::Operaor, ch, pos, OpEnum::LT);
        }
        break;
        case '&':
        {
            // &&
            if (mStream.Match(ch, 1) == true)
            {
                mStream.pos += 2;
                return Token(TkKind::Operaor, "&&", pos, OpEnum::And);
            }
            // &=
            if (mStream.Match('=', 1) == true)
            {
                mStream.pos += 2;
                return Token(TkKind::Operaor, "&=", pos, OpEnum::BitAndAssign);
            }
            mStream.pos += 1;
            return Token(TkKind::Operaor, ch, pos, OpEnum::BitAnd);
        }
        case '^':
        {
            // ^=
            if (mStream.Match('=', 1) == true)
            {
                mStream.pos += 2;
                return Token(TkKind::Operaor, "^=", pos, OpEnum::BitXOrAssign);
            }
            mStream.pos += 1;
            return Token(TkKind::Operaor, ch, pos, OpEnum::BitXOr);
        }
        case '|':
        {
            // ||
            if (mStream.Match(ch, 1) == true)
            {
                mStream.pos += 2;
                return Token(TkKind::Operaor, "||", pos, OpEnum::Or);
            }
            // |=
            if (mStream.Match('=', 1) == true)
            {
                mStream.pos += 2;
                return Token(TkKind::Operaor, "|=", pos, OpEnum::BitOrAssign);
            }
            mStream.pos += 1;
            return Token(TkKind::Operaor, ch, pos, OpEnum::BitOr);
        }
        case '~':
        {
            // ~=
            if (mStream.Match('=', 1) == true)
            {
                mStream.pos += 2;
                return Token(TkKind::Operaor, "~=", pos, OpEnum::BitNotAssign);
            }
            mStream.pos += 1;
            return Token(TkKind::Operaor, ch, pos, OpEnum::BitNot);
        }
        break;
        case '!':
        {
            // !=
            if (mStream.Match('=', 1) == true)
            {
                mStream.pos += 2;
                return Token(TkKind::Operaor, "!=", pos, OpEnum::NE);
            }
            mStream.pos += 1;
            return Token(TkKind::Operaor, ch, pos, OpEnum::Not);
        }
        break;
        case '+':
        {
            // +=
            if (mStream.Match('=', 1) == true)
            {
                mStream.pos += 2;
                return Token(TkKind::Operaor, "+=", pos, OpEnum::PlusAssign);
            } // ++
            if (mStream.Match('+', 1) == true)
            {
                mStream.pos += 2;
                return Token(TkKind::Operaor, "++", pos, OpEnum::Plus2);
            }
            mStream.pos += 1;
            return Token(TkKind::Operaor, ch, pos, OpEnum::Plus);
        }
        break;
        case '-':
        {
            // -=
            if (mStream.Match('=', 1) == true)
            {
                mStream.pos += 2;
                return Token(TkKind::Operaor, "-=", pos, OpEnum::MinusAssign);
            } // --
            if (mStream.Match('-', 1) == true)
            {
                mStream.pos += 2;
                return Token(TkKind::Operaor, "--", pos, OpEnum::Minus2);
            }
            mStream.pos += 1;
            return Token(TkKind::Operaor, ch, pos, OpEnum::Minus);
        }
        break;
        case '*':
        {
            // *=
            if (mStream.Match('=', 1) == true)
            {
                mStream.pos += 2;
                return Token(TkKind::Operaor, "*=", pos, OpEnum::MulAssign);
            }
            mStream.pos += 1;
            return Token(TkKind::Operaor, ch, pos, OpEnum::Multiply);
        }
        break;
        case '/':
        {
            // 单行注释
            if (mStream.Match('/', 1) == true)
            {
                return GetSinglineComment();
            }
            // 多行注释
            if (mStream.Match('*', 1) == true)
            {
                return GetMultilineComment();
            }
            // /=
            if (mStream.Match('=', 1) == true)
            {
                mStream.pos += 2;
                return Token(TkKind::Operaor, "/=", pos, OpEnum::DivAssign);
            }
            mStream.pos += 1;
            return Token(TkKind::Operaor, ch, pos, OpEnum::Divide);
        }
        break;
        case '%':
        {
            // %=
            if (mStream.Match('=', 1) == true)
            {
                mStream.pos += 2;
                return Token(TkKind::Operaor, "%=", pos, OpEnum::ModAssign);
            }
            mStream.pos += 1;
            return Token(TkKind::Operaor, ch, pos, OpEnum::Modulus);
        }
        break;
        case '=':
        {
            // ==
            if (mStream.Match('=', 1) == true)
            {
                mStream.pos += 2;
                return Token(TkKind::Operaor, "==", pos, OpEnum::EQ);
            }
            mStream.pos += 1;
            return Token(TkKind::Operaor, ch, pos, OpEnum::Assign);
        }
        break;
        }
        Error("未能识别词法规则！%s\n", pos.ToString().c_str());
        return Token(TkKind::Error, ch, pos);
    }
    Token Scanner::GetSinglineComment()
    {
        mStream.pos += 2;
        auto pos = GetCurPos();
        Token tok(TkKind::Notes, pos);
        while (1)
        {
            auto ch = mStream.Next();
            if (ch == '\n' || ch == '\0')
                break;
            tok.text.push_back(ch);
        }
        return tok;
    }
    Token Scanner::GetMultilineComment()
    {
        mStream.pos += 2;
        auto pos = GetCurPos();
        Token tok(TkKind::MultiNotes, pos);

        while (1)
        {
            auto ch = mStream.Next();
            if (ch == '\0')
            {
                Error("文件意外结束,缺少符号'*/':%s", pos.ToString().c_str());
                return Token(TkKind::Error, pos);
            }
            if (ch == '*' && mStream.Match('/', 0))
            {
                ch = mStream.Next();
                break;
            }
            tok.text.push_back(ch);
        }
        return tok;
    }
    Token Scanner::GetString(bool escape)
    {
        auto ch = mStream.Next();
        auto pos = GetCurPos();
        Token tok(TkKind::StringLiteral, pos);

        while (1)
        {
            auto ch = mStream.Next();

            // 文件结束
            if (ch == '\0')
            {
                Error("文件意外结束,缺少符号'\"':%s", pos.ToString().c_str());
                return Token(TkKind::Error, pos);
            }
            if (escape == false)
            {
                // 过滤显式多行
                if (ch == '\r' || ch == '\n')
                    continue;
                // 转义字符
                if (ch == '\\')
                {
                    ch = mStream.Peek();
                    switch (ch)
                    {
                    case '\\':
                        tok.text.push_back('\\');
                        mStream.Next();
                        continue;
                    case '"':
                        tok.text.push_back('"');
                        mStream.Next();
                        continue;
                    case '0':
                        tok.text.push_back('\0');
                        mStream.Next();
                        continue;
                    case 'a':
                        tok.text.push_back('\a');
                        mStream.Next();
                        continue;
                    case 'b':
                        tok.text.push_back('\b');
                        mStream.Next();
                        continue;
                    case 'f':
                        tok.text.push_back('\f');
                        mStream.Next();
                        continue;
                    case 'n':
                        tok.text.push_back('\n');
                        mStream.Next();
                        continue;
                    case 'r':
                        tok.text.push_back('\r');
                        mStream.Next();
                        continue;
                    case 't':
                        tok.text.push_back('\t');
                        mStream.Next();
                        continue;
                    case 'v':
                        tok.text.push_back('\v');
                        mStream.Next();
                        continue;
                    }
                    Waring("未知转义字符：\\%c\n", ch);
                    tok.text.push_back('\\');
                    continue;
                }
                // 符号结束
            }
            if (ch == '"')
                break;

            tok.text.push_back(ch);
        }
        return tok;
    }
    Token Scanner::GetIdent()
    {
        auto pos = GetCurPos();
        std::string szBuf;
        while (mStream.Eof() == false &&
               mStream.IsIdent() == true)
        {
            szBuf.push_back(mStream.Next());
        }
        auto find = MapTable::FindKeyEnum(szBuf);
        if (find != KeyEnum::Unknown)
            return Token(TkKind::KeyWord, szBuf, pos, find);
        return Token(TkKind::Identifier, szBuf, pos);
    }
    Token Scanner::GetMacro()
    {
        auto pos = GetCurPos();
        std::string szBuf;
        mStream.Next();
        szBuf.push_back('@');
        while (mStream.Eof() == false &&
               mStream.IsIdent() == true)
        {
            szBuf.push_back(mStream.Next());
        }
        auto find = MapTable::FindMacroEnum(szBuf);
        if (find == MacroEnum::Unknown)
        {
            Error("%s: 未知宏关键字 '%s'\n", pos.ToString().c_str(), szBuf.c_str());
            return Token(TkKind::Error, szBuf, pos);
        }

        return Token(TkKind::Macro, szBuf, pos, find);
    }
    Token Scanner::GetChar()
    {
        auto ch = mStream.Next();
        auto pos = GetCurPos();
        Token tok(TkKind::CharLiteral, pos);
        while (1)
        {
            // 文件结束
            if (mStream.IsWhiteSpace() == true || mStream.Match('\0', 0) == true)
            {
                Error("缺少符号'\'':%s", pos.ToString().c_str());
                return Token(TkKind::Error, pos);
            }
            auto ch = mStream.Next();
            // 转义字符
            if (ch == '\\')
            {
                ch = mStream.Peek();
                switch (ch)
                {
                case '\\':
                    tok.text.push_back('\\');
                    mStream.Next();
                    continue;
                case '"':
                    tok.text.push_back('"');
                    mStream.Next();
                    continue;
                case '0':
                    tok.text.push_back('\0');
                    mStream.Next();
                    continue;
                case 'a':
                    tok.text.push_back('\a');
                    mStream.Next();
                    continue;
                case 'b':
                    tok.text.push_back('\b');
                    mStream.Next();
                    continue;
                case 'f':
                    tok.text.push_back('\f');
                    mStream.Next();
                    continue;
                case 'n':
                    tok.text.push_back('\n');
                    mStream.Next();
                    continue;
                case 'r':
                    tok.text.push_back('\r');
                    mStream.Next();
                    continue;
                case 't':
                    tok.text.push_back('\t');
                    mStream.Next();
                    continue;
                case 'v':
                    tok.text.push_back('\v');
                    mStream.Next();
                    continue;
                }
                Waring("未知转义字符：\\%c\n", ch);
                tok.text.push_back('\\');
                continue;
            }
            // 符号结束

            if (ch == '\'')
                break;

            tok.text.push_back(ch);
        }
        return tok;
    }
    Token Scanner::GetDigit()
    {
        auto pos = GetCurPos();
        auto ch = mStream.Peek();

        if (ch == '0')
        {
            mStream.Next();

            auto ch1 = mStream.Peek();
            switch (ch1)
            {
                // 16进制
            case 'X':
            case 'x':
            {
                std::string szHex = "0x";
                mStream.Next();
                while (mStream.IsHex() == true)
                {
                    szHex.push_back(mStream.Next());
                }
                if (szHex.size() == 2)
                {
                    Error("请输入完整的十六进制!\n");
                    return Token(TkKind::Error, "", pos);
                }
                return Token(TkKind::IntegerLiteral, szHex, pos);
            }
            // 2进制
            break;
            case 'B':
            case 'b':
            {
                std::string szBin = "0b";
                mStream.Next();
                while (mStream.IsBin() == true)
                {
                    szBin.push_back(mStream.Next());
                }
                if (szBin.size() == 2)
                {
                    Error("请输入完整的二进制!\n");
                    return Token(TkKind::Error, "", pos);
                }
                return Token(TkKind::IntegerLiteral, szBin, pos);
            }
            break;
            }
            return Token(TkKind::IntegerLiteral, "0", pos);
        }

        std::string szBuf;
        while (mStream.IsDigital() || mStream.Peek() == '.')
        {
            if (mStream.Peek() == '.')
            {
                szBuf.push_back(mStream.Next());
                auto tok = GetFltNumber(szBuf);
                tok.pos = pos;
                return tok;
            }
            szBuf.push_back(mStream.Next());
        }
        return Token(TkKind::IntegerLiteral, szBuf, pos);
    }
    Token Scanner::GetFltNumber(std::string &szBuf)
    {
        while (mStream.IsDigital())
        {
            szBuf.push_back(mStream.Next());
        }
        auto pos = GetCurPos();
        return Token(TkKind::FloatLiteral, szBuf, pos);
    }
}
