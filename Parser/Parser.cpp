#include "Parser.h"
#include <Utils/FileHelp.h>
#include <Utils/MapTable.h>

namespace air
{
    Parser::Parser(FileUnit &unit, StringPool &pool, const std::string &szPath)
        : mUnit(unit), mPool(pool), mScope(ScopeEnum::Public)
    {
        // 读取文件
        if (ReadFile(szPath, mLexer.GetStream()) == false)
            throw ErrorExpception("打开文件失败!");

        // 打印解析到哪个文件
        Print("编译 %s\n", unit.mFile->c_str());

        // 解析包名
        GetPackage();
        // 解析导入项依赖
        GetImport();

        // 正式解析声明
        Token tok;
        while (true)
        {
            tok = mLexer.GetNext();

            // 文件结束
            if (tok.kind == TkKind::Eof)
                return;

            // 多余的分号：不影响代码的正确性
            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::SemiColon)
                continue;

            mLexer.BackTok(tok);
            // 解析声明
            unit.mAstTree.push_back(Declare(mScope));
        }
    }
    // 语法 package id [.id]*;
    void Parser::GetPackage()
    {
        auto tok = mLexer.GetNext();
        if (tok.kind != TkKind::KeyWord || tok.code.key != KeyEnum::Package)
        {
            Error("line:%d\n", tok.pos.mLine);
            Error("tok:%s\n", tok.text.c_str());
            throw ErrorExpception("缺少包声明!");
        }

        std::string szPkg;
        while (true)
        {
            tok = mLexer.GetNext();
            if (tok.kind != TkKind::Identifier)
            {
                Error("line:%d\n", tok.pos.mLine);
                Error("tok:%s\n", tok.text.c_str());
                throw ErrorExpception("缺少包名称!");
            }
            szPkg += tok.text;
            tok = mLexer.GetNext();
            if (tok.kind == TkKind::Operaor && tok.code.op == OpEnum::Dot)
            {
                szPkg.push_back('.');
                continue;
            }
            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::SemiColon)
                break;
            Error("line:%d\n", tok.pos.mLine);
            Error("tok:%s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号 ';' !");
        }
        mUnit.mPackage = mPool.RefString(szPkg);
    }
    // 语法1 import id="file.ext";
    // 语法2 import{ id="file.ext"; }
    void Parser::GetImport()
    {
        while (true)
        {
            auto tok = mLexer.GetNext();
            if (tok.kind != TkKind::KeyWord || tok.code.key != KeyEnum::Import)
            {
                mLexer.BackTok(tok);
                return;
            }
            tok = mLexer.GetNext();
            if (tok.kind == TkKind::Identifier)
            {
                mLexer.BackTok(tok);
                GetImportItem();
                continue;
            }

            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::OpenBrace)
            {
                while (true)
                {
                    GetImportItem();
                    tok = mLexer.GetNext();
                    if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseBrace)
                        break;
                    mLexer.BackTok(tok);
                }
                continue;
            }
            Error("line:%d\n", tok.pos.mLine);
            Error("tok:%s\n", tok.text.c_str());
            throw ErrorExpception("缺少导入名称！");
        }
    }
    // 语法 id="string";
    void Parser::GetImportItem()
    {
        std::string szFile, szName;
        auto tok = mLexer.GetNext();
        if (tok.kind != TkKind::Identifier)
        {
            Error("line:%d\n", tok.pos.mLine);
            Error("tok:%s\n", tok.text.c_str());
            throw ErrorExpception("缺少依赖别名！");
        }

        szName = tok.text;

        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Operaor || tok.code.op != OpEnum::Assign)
        {
            Error("line:%d\n", tok.pos.mLine);
            Error("tok:%s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号 '='！");
        }

        tok = mLexer.GetNext();
        if (tok.kind != TkKind::StringLiteral)
        {
            Error("line:%d\n", tok.pos.mLine);
            Error("tok:%s\n", tok.text.c_str());
            throw ErrorExpception("缺少依赖文件路径！");
        }
        szFile = tok.text;

        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::SemiColon)
        {
            Error("line:%d\n", tok.pos.mLine);
            Error("tok:%s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号 ';'！");
        }

        auto name = mPool.RefString(szName);
        auto file = mPool.RefString(szFile);

        if (mUnit.mImports.mNames.insert(name).second == false)
        {
            Error("名称 '%s' 重复\n", szName.c_str());
            throw ErrorExpception("依赖别名重复！");
        }
        if (mUnit.mImports.mFiles.insert(file).second == false)
            Waring("文件 '%s' 重复导入!\n", szFile.c_str());

        mUnit.mImports.mItems.insert({name, file});
    }

    AstDeclRef Parser::Declare(ScopeEnum &defaultScope)
    {
        AstFlag flag;
        flag.mFlag = 0;
        AstType type;
        ScopeEnum scope;

        // 1.解析作用域标记
        Front3PScope(defaultScope, scope);
        flag.mScope = scope;
        // 2.解析类型声明
        auto tok = mLexer.GetNext();
        if (tok.kind == TkKind::KeyWord)
        {
            switch (tok.code.key)
            {
            case KeyEnum::Enum:
                return Enum(scope);
                break;
            case KeyEnum::Struct:
                return Struct(scope);
                break;
            case KeyEnum::Union:
                return Union(scope);
                break;
            case KeyEnum::Entrust:
                return Entrust(scope);
                break;
            case KeyEnum::Interface:
                return Interface(scope);
                break;
            case KeyEnum::Class:
                return Class(scope);
                break;
            }
        }
        mLexer.BackTok(tok);
        // 2.解析类型
        Type(type);
        // 3.解析名称

        // 4.判断是否是函数

        return AstDeclRef();
    }
    // 3P: public | private | protected
    void Parser::Front3PScope(ScopeEnum &defaultScope, ScopeEnum &currentScope)
    {
    _Start_:
        auto tok = mLexer.GetNext();

        // 是关键字
        if (tok.kind == TkKind::KeyWord)
        {
            switch (tok.code.key)
            {
            case KeyEnum::Public:
                currentScope = ScopeEnum::Public;
                break;
            case KeyEnum::Private:
                currentScope = ScopeEnum::Private;
                break;
            case KeyEnum::Protected:
                currentScope = ScopeEnum::Protected;
                break;
            // 默认的全局作用域
            default:
            {
                currentScope = mScope;
                goto _End_;
            }
            }
        }
        // 判断全局设定
        tok = mLexer.GetNext();
        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Colon)
        {
            // 更新全局设定
            defaultScope = currentScope;
            // 再次设定
            goto _Start_;
        }

    _End_:
        mLexer.BackTok(tok);
        return;
    }
    // static
    void Parser::FrontFlag(AstFlag &flag)
    {
        while (true)
        {
            auto tok = mLexer.GetNext();
            if (tok.kind == TkKind::KeyWord)
            {
                switch (tok.code.key)
                {
                case KeyEnum::Static:
                    flag.mStatic = 1;
                    break;

                default:
                    mLexer.BackTok(tok);
                    return;
                }

                return;
            }

            mLexer.BackTok(tok);
            return;
        }
    }

    void Parser::PostFlag(AstFlag &flag)
    {
    }
    // [const ] file.type.type [] &
    void Parser::Type(AstType &type)
    {
        auto tok = mLexer.GetNext();
        // 查看是否为const
        if (tok.kind == TkKind::KeyWord && tok.code.key == KeyEnum::Const)
        {
            type.mConst = 1;
            tok = mLexer.GetNext();
        }
        // 解析类型名称
        {
            // 基本类型
            if (tok.kind == TkKind::KeyWord &&
                (KeyEnum::Void <= tok.code.key &&
                 tok.code.key <= KeyEnum::Char))
            {
                type.mTypeStr.push_back(mPool.RefString(tok.text));
            }
            // 自定义类型 file.class.class
            else if (tok.kind == TkKind::Identifier)
            {
                while (true)
                {
                    if (tok.kind == TkKind::Identifier)
                        type.mTypeStr.push_back(mPool.RefString(tok.text));
                    else
                    {
                        Error("line:%d\n", tok.pos.mLine);
                        Error("tok:%s\n", tok.text.c_str());
                        throw ErrorExpception("缺少标识符！");
                    }
                    tok = mLexer.GetNext();
                    if (tok.kind == TkKind::Operaor && tok.code.op == OpEnum::Dot)
                    {
                        tok = mLexer.GetNext();
                        continue;
                    }
                    mLexer.BackTok(tok);
                    break;
                }
            }
            else
            {
                Error("line:%d\n", tok.pos.mLine);
                Error("tok:%s\n", tok.text.c_str());
                throw ErrorExpception("缺少标识符！");
            }
        }
        tok = mLexer.GetNext();
        // 查看是否是动态数组
        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::OpenBracket)
        {
            tok = mLexer.GetNext();
            // 块数组
            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Comma)
            {
                int colcnt = 2;
                while (true)
                {
                    tok = mLexer.GetNext();
                    // 结束
                    if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseBracket)
                        break;
                    // 再加一维
                    if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Comma)
                    {
                        ++colcnt;
                        continue;
                    }
                    // 语法错误
                    Error("line:%d\n", tok.pos.mLine);
                    Error("tok:%s\n", tok.text.c_str());
                    throw ErrorExpception("缺少符号: ']' ！");
                }
                type.mArrDynCol = colcnt;
                type.mArrBlk = 1;
                // 验证维度
                if (colcnt > 3)
                {
                    Error("line:%d\ncol:%d\n", tok.pos.mLine, colcnt);
                    throw ErrorExpception("数组维度>3 ！");
                }
                tok = mLexer.GetNext(); // 为查看是否是引用获取单词
            }
            // 一维块数组或者多维交错数组
            else if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseBracket)
            {
                tok = mLexer.GetNext();
                // 交错数组
                if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::OpenBracket)
                {
                    int colcnt = 1;
                    while (true)
                    {
                        tok = mLexer.GetNext();
                        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseBracket)
                        {
                            ++colcnt;
                            tok = mLexer.GetNext();
                            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::OpenBracket)
                                continue;
                            else
                                break;
                        }
                        // 语法错误
                        Error("line:%d\n", tok.pos.mLine);
                        Error("tok:%s\n", tok.text.c_str());
                        throw ErrorExpception("缺少符号: ']' ！");
                    }
                    type.mArrBlk == 0;
                    type.mArrDynCol = colcnt;
                }
                // 一维块数组
                else
                {
                    type.mArrDynCol = 1;
                    type.mArrBlk = 1;
                }
            }
            else
            {
                // 语法错误
                Error("line:%d\n", tok.pos.mLine);
                Error("tok:%s\n", tok.text.c_str());
                throw ErrorExpception("缺少符号: ']' ！");
            }
        }
        // 查看是否引用
        if (tok.kind == TkKind::Operaor && tok.code.op == OpEnum::BitAnd)
        {
            type.mReference = 1;
        }
        else
        {
            mLexer.BackTok(tok);
        }
    }

    // 语法：enum name:basetype{ name=exp [,name=exp]*}
    AstDeclRef Parser::Enum(ScopeEnum scope)
    {
        auto tok = mLexer.GetNext();
        // 解析名称
        if (tok.kind != TkKind::Identifier)
        {
            Error("line:%d\n", tok.pos.mLine);
            Error("tok:%s\n", tok.text.c_str());
            throw ErrorExpception("缺少枚举类型名称 !");
        }
        EnumDecl *decins = nullptr;
        auto decl = GenDecl(decins);
        decins->mScope = scope;
        decins->mName = mPool.RefString(tok.text);
        decins->mStartPos = tok.pos;
        // 解析基本类型
        tok = mLexer.GetNext();
        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Colon)
        {
            tok = mLexer.GetNext();
            // 基本类型
            if (tok.kind == TkKind::KeyWord &&
                (KeyEnum::Int8 <= tok.code.key &&
                 tok.code.key <= KeyEnum::UInt))
            {
                decins->mBaseType = mPool.RefString(tok.text);
            }
            else if (tok.kind == TkKind::Identifier)
            {
                decins->mBaseType = mPool.RefString(tok.text);
            }
            else
            {
                Error("line:%d\n", tok.pos.mLine);
                Error("tok:%s\n", tok.text.c_str());
                throw ErrorExpception("缺少合法的类型名称 !");
            }
            tok = mLexer.GetNext();
        }
        // 确保 {
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenBrace)
        {
            Error("line:%d\n", tok.pos.mLine);
            Error("tok:%s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号 ‘{’ !");
        }
        // 解析 枚举项声明
        while (true)
        {
            tok = mLexer.GetNext();
            // 结束
            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseBrace)
            {
                decins->mEndPos = tok.pos;
                break;
            }
            // 枚举项名称
            if (tok.kind != TkKind::Identifier)
            {
                Error("line:%d\n", tok.pos.mLine);
                Error("tok:%s\n", tok.text.c_str());
                throw ErrorExpception("缺少枚举项名称 !");
            }
            auto &item = decins->mEnums.emplace_back();
            item.mName = mPool.RefString(tok.text);
            // 枚举项值
            tok = mLexer.GetNext();
            if (tok.kind == TkKind::Operaor && tok.code.op == OpEnum::Assign)
            {
                item.mValExp = Expression();
                tok = mLexer.GetNext();
            }
            // 逗号
            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Comma)
                continue;
            // 结束
            else if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseBrace)
            {
                decins->mEndPos = tok.pos;
                break;
            }
            else
            {
                Error("line:%d\n", tok.pos.mLine);
                Error("tok:%s\n", tok.text.c_str());
                throw ErrorExpception("缺少符号 '}' !");
            }
        }

        return decl;
    }

    AstDeclRef Parser::Struct(ScopeEnum scope)
    {
        return AstDeclRef();
    }

    AstDeclRef Parser::Union(ScopeEnum scope)
    {
        return AstDeclRef();
    }

    AstDeclRef Parser::Entrust(ScopeEnum scope)
    {
        return AstDeclRef();
    }

    AstDeclRef Parser::Interface(ScopeEnum scope)
    {
        return AstDeclRef();
    }

    AstDeclRef Parser::Class(ScopeEnum scope)
    {
        return AstDeclRef();
    }

    AstExpRef Parser::BasicExpression()
    {
        auto tok = mLexer.GetNext();

        switch (tok.kind)
        {
            // 变量、函数调用、数组访问
        case TkKind::Identifier:
        {
            auto ID = mPool.RefString(tok.text);
            auto startpos = tok.pos;

            tok = mLexer.GetNext();

            // 函数调用
            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::OpenParen)
            {
                return CallFunExp(ID, startpos);
            }
            // 数组访问
            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::OpenBracket)
            {
                return ArrayIndexExp(ID, startpos);
            }
            // 变量
            mLexer.BackTok(tok);

            IdentExp *exp = nullptr;
            auto expres = GenExp(exp, ID);
            exp->mStartPos = startpos;
            exp->mEndPos = tok.pos;
            return expres;
        }
        break;

        case TkKind::KeyWord:
        {
            switch (tok.code.key)
            { // 类型转换:cast<type>(exp)
            case KeyEnum::Cast:
            {
                CastExp *exp = nullptr;
                auto expres = GenExp(exp);
                exp->mStartPos = tok.pos;

                // 解析类型<type>
                {
                    tok = mLexer.GetNext();
                    if (tok.kind != TkKind::Operaor || tok.code.op != OpEnum::LT)
                    {
                        Error("line:%d\n", tok.pos.mLine);
                        Error("tok:%s\n", tok.text.c_str());
                        throw ErrorExpception("缺少符号'<' ！");
                    }

                    Type(exp->mType);

                    if (tok.kind != TkKind::Operaor || tok.code.op != OpEnum::GT)
                    {
                        Error("line:%d\n", tok.pos.mLine);
                        Error("tok:%s\n", tok.text.c_str());
                        throw ErrorExpception("缺少符号'>' ！");
                    }
                }
                // 解析表达式 (exp)
                {
                    tok = mLexer.GetNext();
                    if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenParen)
                    {
                        Error("line:%d\n", tok.pos.mLine);
                        Error("tok:%s\n", tok.text.c_str());
                        throw ErrorExpception("缺少符号'(' ！");
                    }

                    exp->mExp = Expression();

                    tok = mLexer.GetNext();
                    if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::CloseParen)
                    {
                        Error("line:%d\n", tok.pos.mLine);
                        Error("tok:%s\n", tok.text.c_str());
                        throw ErrorExpception("缺少符号')' ！");
                    }
                    exp->mEndPos = mLexer.GetCurPos();
                }
                return expres;
            }
            break;
            // this常量
            case KeyEnum::This:
            case KeyEnum::Super:
            {
                ThisSuperExp *exp = nullptr;
                auto expres = GenExp(exp, tok.code.key == KeyEnum::This);
                exp->mStartPos = tok.pos;
                exp->mEndPos = mLexer.GetCurPos();
                return expres;
            }
            break;
            // bool常量
            case KeyEnum::True:
            case KeyEnum::False:
            {
                BoolExp *exp = nullptr;
                auto expres = GenExp(exp, tok.code.key == KeyEnum::True);
                exp->mStartPos = tok.pos;
                exp->mEndPos = mLexer.GetCurPos();
                return expres;
            }
            break;
            // 空指针常量
            case KeyEnum::Nullptr:
            {
                NullExp *exp = nullptr;
                auto expres = GenExp(exp);
                exp->mStartPos = tok.pos;
                exp->mEndPos = mLexer.GetCurPos();
                return expres;
            }
            break;

            default:
                goto _Error_;
            }
        }
        break;

        case TkKind::Seperator:
        {
            switch (tok.code.sp)
            {
                // 括号表达式
            case SpEnum::OpenParen:
            {
                ParenExp *exp = nullptr;
                auto expres = GenExp(exp);
                exp->mStartPos = tok.pos;
                exp->mExp = Expression();
                tok = mLexer.GetNext();
                if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::CloseParen)
                {
                    Error("line:%d\n", tok.pos.mLine);
                    Error("tok:%s\n", tok.text.c_str());
                    throw ErrorExpception("缺少符号')' ！");
                }
                exp->mEndPos = mLexer.GetCurPos();
                return expres;
            }
            break;

            default:
                goto _Error_;
            }
        }
        break;
        // 字符常量
        case TkKind::CharLiteral:
        {
            CharExp *exp = nullptr;
            auto expres = GenExp(exp);
            exp->mStartPos = tok.pos;
            exp->mEndPos = mLexer.GetCurPos();
            tok.GetCharVal(exp->mVal);
            return expres;
        }
        break;
        // 字符串常量
        case TkKind::StringLiteral:
        {
            StringExp *exp = nullptr;
            auto expres = GenExp(exp);
            exp->mStartPos = tok.pos;
            exp->mEndPos = mLexer.GetCurPos();
            exp->mVal = mPool.RefString(tok.text);
            return expres;
        }
        break;
        // 整数常量
        case TkKind::IntegerLiteral:
        {
            uint64_t val = 0;
            StringRef type = nullptr;
            // 有符号常量表达式
            if (tok.GetIntVal(val, type))
            {
                IntExp *exp = nullptr;
                auto expres = GenExp(exp, type, val);
                exp->mStartPos = tok.pos;
                exp->mEndPos = mLexer.GetCurPos();
                return expres;
            }
            // 无符号常量表达式
            UintExp *exp = nullptr;
            auto expres = GenExp(exp, type, val);
            exp->mStartPos = tok.pos;
            exp->mEndPos = mLexer.GetCurPos();
            return expres;
        }
        break;
        // 浮点常量
        case TkKind::FloatLiteral:
        {
            FloatExp *exp = nullptr;
            auto expres = GenExp(exp);
            exp->mStartPos = tok.pos;
            exp->mEndPos = mLexer.GetCurPos();
            tok.GetFltVal(exp->mValue, exp->mType);
            return expres;
        }
        break;
        }
    _Error_:
    {
        Error("line:%d\n", tok.pos.mLine);
        Error("tok:%s\n", tok.text.c_str());
        throw ErrorExpception("缺少表达式 ！");
    }
        return AstExpRef();
    }
    // id(exp,exp)
    AstExpRef Parser::CallFunExp(StringRef id, TkPos start)
    {
        FunCallExp *exp = nullptr;
        auto expres = GenExp(exp, id);
        exp->mStartPos = start;
        // 解析参数
        while (true)
        {
            auto tok = mLexer.GetNext();
            // 结束符 ）
            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseParen)
                break;
            mLexer.BackTok(tok);
            exp->mArgs.push_back(Expression());
            tok = mLexer.GetNext();
            // 逗号 ，
            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Comma)
                continue;
            // 结束符 ）
            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseParen)
                break;
            // 错误
            Error("line:%d\n", tok.pos.mLine);
            Error("tok:%s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号 ')' ！");
        }

        exp->mStartPos = mLexer.GetCurPos();
        return expres;
    }

    // id[exp,exp] 或 id[exp][exp]
    AstExpRef Parser::ArrayIndexExp(StringRef id, TkPos start)
    {
        throw ErrorExpception("暂不支持数组访问表达式");
        ArrayExp *exp = nullptr;
        auto expres = GenExp(exp, id);
        exp->mStartPos = start;
        Token tok;

        // 解析块索引表达式:[exp,exp]
        while (true)
        {
            exp->mIndexs.push_back(Expression());
            tok = mLexer.GetNext();
            // 逗号 ，
            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Comma)
                continue;
            // 结束符 ]
            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseBracket)
                break;
            // 错误
            Error("line:%d\n", tok.pos.mLine);
            Error("tok:%s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号 ']' ！");
        }
        exp->mEndPos = mLexer.GetCurPos();
        // 验证是否是交错数组
        if (exp->mIndexs.size() == 1)
        {
            // 可能是交错数组: [exp]
            tok = mLexer.GetNext();
            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::OpenBracket)
            {
                while (true)
                {
                    exp->mIndexs.push_back(Expression());
                    tok = mLexer.GetNext();
                    // 结束符 ]
                    if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseBracket)
                    {
                        tok = mLexer.GetNext();
                        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::OpenBracket)
                            continue;
                        break;
                    }
                    // 错误
                    Error("line:%d\n", tok.pos.mLine);
                    Error("tok:%s\n", tok.text.c_str());
                    throw ErrorExpception("缺少符号 ']' ！");
                }
                exp->mEndPos = mLexer.GetCurPos();
                exp->mBlock = exp->mIndexs.size() == 1;
            }
            else
            {
                mLexer.BackTok(tok);
            }
        }

        return expres;
    }
    AstExpRef Parser::UnaryExpression()
    {
        auto tok = mLexer.GetNext();
        // 前置一元操作符
        if (tok.kind == TkKind::Operaor &&
            (tok.code.op == OpEnum::Plus ||  // +
             tok.code.op == OpEnum::Plus2 || // ++
             tok.code.op == OpEnum::Minus || // -
             tok.code.op == OpEnum::Minus2)) // --
        {
            auto exp = UnaryExpression();
            UnaryExp *una = nullptr;
            auto unaexp = GenExp(una, true, tok.code.op, exp);
            /*una->mPrev = true;
            una->mOp = tok.code.op;
           una->mExp = exp;*/
            una->mStartPos = tok.pos;
            una->mEndPos = exp->mEndPos;
            return unaexp;
        }
        mLexer.BackTok(tok);

        // 后置一元操作符
        auto basic = BasicExpression();
        tok = mLexer.GetNext();
        if (tok.kind == TkKind::Operaor &&
            (tok.code.op == OpEnum::Plus2 || // ++
             tok.code.op == OpEnum::Minus2)) // --
        {
            UnaryExp *una = nullptr;
            auto unaexp = GenExp(una, false, tok.code.op, basic);
            /*una->mPrev = false;
            una->mOp = tok.code.op;
            una->mExp = basic;*/
            una->mStartPos = basic->mStartPos;
            una->mEndPos = tok.pos;
            return unaexp;
        }
        // 成员调用
        if (tok.kind == TkKind::Operaor && tok.code.op == OpEnum::Dot)
        {
            DotCallExp *dot = nullptr;
            auto exp = GenExp(dot);
            exp->mStartPos = basic->mStartPos;
            while (true)
            {
                // 检查表达式类别
                switch (basic->GetKind())
                {
                case ExpKind::Identity:
                case ExpKind::Array:
                case ExpKind::FunCall:
                {
                    dot->mItems.push_back(basic);
                }
                break;

                case ExpKind::This:
                case ExpKind::Super:
                {
                    if (dot->mItems.empty() == true)
                    {
                        dot->mItems.push_back(basic);
                        break;
                    }
                }
                default:
                {
                    Error("line:%d\n", tok.pos.mLine);
                    Error("tok:%s\n", tok.text.c_str());
                    throw ErrorExpception("缺少成员 ！");
                }
                break;
                }
                tok = mLexer.GetNext();
                if (tok.kind == TkKind::Operaor && tok.code.op == OpEnum::Dot)
                {
                    basic = BasicExpression();
                    continue;
                }
                mLexer.BackTok(tok);
                break;
            }
            exp->mEndPos = basic->mEndPos;
            return exp;
        }
        mLexer.BackTok(tok);
        return basic;
    }
    AstExpRef Parser::BinaryExpression(int opPriority)
    {
        // 1.先解析一个一元表达式
        auto leftExp = UnaryExpression();

        // 获取操作符
        auto tok = mLexer.GetNext();
        int32_t priority = -1;
        // 不是操作符
        if (tok.kind != TkKind::Operaor)
        {
            mLexer.BackTok(tok);
            return leftExp;
        }
        // 查找二元操作符
        priority = MapTable::FindOpPriority(tok.code.op);
        if (priority == -1)
        {
            // 是不是？：表达式
            if (tok.code.op == OpEnum::Question)
                return TernaryExpression(leftExp);
            mLexer.BackTok(tok);
            return leftExp;
        }
        // 记录一下二元操作符
        OpEnum op = tok.code.op;

        // 循环组成二元表达式
        while (priority > opPriority)
        {
            auto rightExp = BinaryExpression(priority);

            // 生成二元结构
            BinaryExp *bin = nullptr;
            auto binExp = GenExp(bin, op, leftExp, rightExp);
            /*bin->mLeft = leftExp;
            bin->mRight = rightExp;
            bin->mOp = op;*/
            bin->mStartPos = leftExp->mStartPos;
            bin->mEndPos = rightExp->mEndPos;

            leftExp = binExp;

            // 继续查看：二元操作
            tok = mLexer.GetNext();
            if (tok.kind != TkKind::Operaor)
            {
                mLexer.BackTok(tok);
                return leftExp;
            }
            op = tok.code.op;
            // 查找二元操作符
            priority = MapTable::FindOpPriority(tok.code.op);
            if (priority == -1)
            {
                // 是不是？：表达式
                if (tok.code.op == OpEnum::Question)
                    return TernaryExpression(leftExp);
                mLexer.BackTok(tok);
                return leftExp;
            }
        }

        return leftExp;
    }
    AstExpRef Parser::TernaryExpression(AstExpRef &cond)
    {
        TernaryExp *ter = nullptr;
        auto exp = GenExp(ter);
        ter->mCond = cond;
        ter->mStartPos = cond->mStartPos;
        ter->mSelect = Expression();
        auto tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::Colon)
        {
            Error("line:%d\n", tok.pos.mLine);
            Error("tok:%s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: ':' ！");
        }
        ter->mSelect2 = Expression();
        ter->mEndPos = ter->mSelect2->mEndPos;
        return exp;
    }
}
