#include "Parser.h"
#include <Utils/FileHelp.h>
#include <Utils/MapTable.h>

namespace air
{
    //--------------------------声明----------------------------

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
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少包声明!");
        }

        std::string szPkg;
        while (true)
        {
            tok = mLexer.GetNext();
            if (tok.kind != TkKind::Identifier)
            {
                Error("    line:  %d\n", tok.pos.mLine);
                Error("    tok:  %s\n", tok.text.c_str());
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
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
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
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
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
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少依赖别名！");
        }

        szName = tok.text;

        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Operaor || tok.code.op != OpEnum::Assign)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号 '='！");
        }

        tok = mLexer.GetNext();
        if (tok.kind != TkKind::StringLiteral)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少依赖文件路径！");
        }
        szFile = tok.text;

        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::SemiColon)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
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
        auto startpos = mLexer.GetCurPos();
        FrontFlag(flag);
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
        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Identifier)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少名称！");
        }
        auto name = mPool.RefString(tok.text);

        // 4.判断是否是函数
        tok = mLexer.GetNext();
        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::OpenParen)
        {
            FunctionDecl *decl = nullptr;
            auto decref = GenDecl(decl);
            decl->mFlag = flag;
            decl->mRetType = type;
            decl->mName = name;
            decl->mStartPos = startpos;
            Function(*decl);
            return decref;
        }

        // 5、解析变量声明

        VariableDecl *decl = nullptr;
        auto decref = GenDecl(decl);
        decl->mFlag = flag;
        decl->mType = type;
        decl->mName = name;
        decl->mStartPos = startpos;

        mLexer.BackTok(tok);
        Variable(*decl);

        return decref;
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
    // static virtual inline
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
                    continue;
                case KeyEnum::Inline:
                    flag.mInline = 1;
                    continue;
                case KeyEnum::Virtual:
                    flag.mVirtual = 1;
                    continue;
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
    // override
    void Parser::PostFlag(AstFlag &flag)
    {
        while (true)
        {
            auto tok = mLexer.GetNext();
            if (tok.kind == TkKind::KeyWord)
            {
                switch (tok.code.key)
                {
                case KeyEnum::Override:
                    flag.mOverride = 1;
                    continue;
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
                type.mName.push_back(mPool.RefString(tok.text));
            }
            // 自定义类型 file.class.class
            else if (tok.kind == TkKind::Identifier)
            {
                while (true)
                {
                    if (tok.kind == TkKind::Identifier)
                        type.mName.push_back(mPool.RefString(tok.text));
                    else
                    {
                        Error("    line:  %d\n", tok.pos.mLine);
                        Error("    tok:  %s\n", tok.text.c_str());
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
                Error("    line:  %d\n", tok.pos.mLine);
                Error("    tok:  %s\n", tok.text.c_str());
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
                    Error("    line:  %d\n", tok.pos.mLine);
                    Error("    tok:  %s\n", tok.text.c_str());
                    throw ErrorExpception("缺少符号: ']' ！");
                }
                type.mArrDynCol = colcnt;
                type.mArrBlk = 1;
                // 验证维度
                if (colcnt > 3)
                {
                    Error("    line:  %d\ncol:%d\n", tok.pos.mLine, colcnt);
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
                        Error("    line:  %d\n", tok.pos.mLine);
                        Error("    tok:  %s\n", tok.text.c_str());
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
                Error("    line:  %d\n", tok.pos.mLine);
                Error("    tok:  %s\n", tok.text.c_str());
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

    void Parser::Variable(VariableDecl &decl)
    {
        auto tok = mLexer.GetNext();
        // 静态数组: id[exp,exp]
        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::OpenBracket)
        {
            while (true)
            {
                decl.mArrCol.push_back(Expression());
                tok = mLexer.GetNext();
                if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Comma)
                    continue;
                if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseBracket)
                    break;
                Error("    line:  %d\n", tok.pos.mLine);
                Error("    tok:  %s\n", tok.text.c_str());
                throw ErrorExpception("缺少符号 ']' ！");
            }
            tok = mLexer.GetNext();
        }

        // 初始值表达式
        if (tok.kind == TkKind::Operaor && tok.code.op == OpEnum::Assign)
        {
            decl.mInitExp = Expression();
            tok = mLexer.GetNext();
        }
        decl.mEndPos = mLexer.GetCurPos();

        // 同行逗号定义变量
        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Comma)
        {
            // name[,]=exp
            while (true)
            {
                VariableDecl *vardecl = nullptr;
                auto decref = GenDecl(vardecl);
                decl.mVarItems.push_back(decref);
                vardecl->mFlag = decl.mFlag;
                vardecl->mType = decl.mType;
                // 获取名称
                tok = mLexer.GetNext();
                if (tok.kind != TkKind::Identifier)
                {
                    Error("    line:  %d\n", tok.pos.mLine);
                    Error("    tok:  %s\n", tok.text.c_str());
                    throw ErrorExpception("缺少名称！");
                }
                vardecl->mName = mPool.RefString(tok.text);
                vardecl->mStartPos = tok.pos;
                // 静态数组
                tok = mLexer.GetNext();
                if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::OpenBracket)
                {
                    while (true)
                    {
                        vardecl->mArrCol.push_back(Expression());
                        tok = mLexer.GetNext();
                        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Comma)
                            continue;
                        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseBracket)
                            break;
                        Error("    line:  %d\n", tok.pos.mLine);
                        Error("    tok:  %s\n", tok.text.c_str());
                        throw ErrorExpception("缺少符号 ']' ！");
                    }
                    tok = mLexer.GetNext();
                }
                // 初始值表达式
                if (tok.kind == TkKind::Operaor && tok.code.op == OpEnum::Assign)
                {
                    vardecl->mInitExp = Expression();
                    tok = mLexer.GetNext();
                }
                vardecl->mEndPos = mLexer.GetCurPos();
                // 继续解析
                if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Comma)
                    continue;
                break;
            }
        }

        // 9、确认分号 ;
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::SemiColon)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号 ';' ！");
        }
    }

    void Parser::Function(FunctionDecl &fundecl)
    {
        // 1、解析参数列表
        ParamList(fundecl.mParams);
        // 2、解析修饰后缀
        PostFlag(fundecl.mFlag);

        auto tok = mLexer.GetNext();
        // 是否是纯虚函数
        if (tok.kind == TkKind::Operaor && tok.code.op == OpEnum::Assign)
        {
            fundecl.mFlag.mPureVir = 1;
            tok = mLexer.GetNext();
            if (tok.kind != TkKind::IntegerLiteral || tok.text != "0")
            {
                Error("    line:  %d\n", tok.pos.mLine);
                Error("    tok:  %s\n", tok.text.c_str());
                throw ErrorExpception("缺少纯虚函数标志符号 '0' ！");
            }

            if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::SemiColon)
                goto _Error_;
            fundecl.mIsDecl = true;
            return;
        }

        // 3、只有声明
        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::SemiColon)
        {
            fundecl.mIsDecl = true;
            return;
        }

        // 4、解析函数体
        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::OpenBrace)
        {
            BlockStatement(fundecl.mFuncBody);
            return;
        }
    _Error_:
        // 错误
        Error("    line:  %d\n", tok.pos.mLine);
        Error("    tok:  %s\n", tok.text.c_str());
        throw ErrorExpception("缺少符号 ';' ！");
    }
    // 参数声明类表：( type name, type name,...)
    void Parser::ParamList(std::vector<ParamItem> &list)
    {
        auto tok = mLexer.GetNext();
        // 无参数
        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseParen)
            return;
        mLexer.BackTok(tok);
        while (true)
        {
            auto &item = list.emplace_back();
            // 解析类型
            Type(item.mType);
            // 解析名称
            tok = mLexer.GetNext();
            if (tok.kind == TkKind::Identifier)
            {
                item.mName = mPool.RefString(tok.text);
                tok = mLexer.GetNext();
            }
            // 逗号，
            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Comma)
                continue;
            // 结束
            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseParen)
                break;
            // 错误
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号 ')' ！");
        }
    }

    // 语法：enum name:basetype{ name=exp [,name=exp]*}
    AstDeclRef Parser::Enum(ScopeEnum scope)
    {
        auto tok = mLexer.GetNext();
        // 解析名称
        if (tok.kind != TkKind::Identifier)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
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
                Error("    line:  %d\n", tok.pos.mLine);
                Error("    tok:  %s\n", tok.text.c_str());
                throw ErrorExpception("缺少合法的类型名称 !");
            }
            tok = mLexer.GetNext();
        }
        // 确保 {
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenBrace)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
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
                Error("    line:  %d\n", tok.pos.mLine);
                Error("    tok:  %s\n", tok.text.c_str());
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
                Error("    line:  %d\n", tok.pos.mLine);
                Error("    tok:  %s\n", tok.text.c_str());
                throw ErrorExpception("缺少符号 '}' !");
            }
        }

        return decl;
    }

    AstMemberRef Parser::MemberItem(ScopeEnum &defaultScope)
    {
        // 先查看是否是 union、struct 成员
        auto tok = mLexer.GetNext();
        if (tok.kind == TkKind::KeyWord)
        {
            switch (tok.code.key)
            {
            case KeyEnum::Union:
                return MemberUnionItem(defaultScope);
                /* case KeyEnum::Struct:
                     return MemberStructItem(defaultScope);*/
            default:
                mLexer.BackTok(tok);
            };
        }
        AstFlag flag;
        flag.mFlag = 0;
        AstType type;
        ScopeEnum scope;

        // 1.解析作用域标记
        Front3PScope(defaultScope, scope);
        flag.mScope = scope;
        auto startpos = mLexer.GetCurPos();
        FrontFlag(flag);
        // 2.解析类型
        Type(type);
        // 3.解析名称
        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Identifier)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少名称！");
        }
        auto name = mPool.RefString(tok.text);

        // 4.判断是否是函数
        tok = mLexer.GetNext();
        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::OpenParen)
        {
            MemberFunc *decl = nullptr;
            auto decref = GenMember(decl);
            decl->mDecl.mFlag = flag;
            decl->mDecl.mRetType = type;
            decl->mDecl.mName = name;
            decl->mDecl.mStartPos = startpos;
            Function(decl->mDecl);
            return decref;
        }

        // 5、解析变量声明

        MemberVar *decl = nullptr;
        auto decref = GenMember(decl);
        decl->mDecl.mFlag = flag;
        decl->mDecl.mType = type;
        decl->mDecl.mName = name;
        decl->mDecl.mStartPos = startpos;

        mLexer.BackTok(tok);
        Variable(decl->mDecl);

        return decref;
    }

    AstMemberRef Parser::MemberVarItem(ScopeEnum &defaultScope)
    {
        AstFlag flag;
        flag.mFlag = 0;
        AstType type;
        ScopeEnum scope;

        // 1.解析作用域标记
        Front3PScope(defaultScope, scope);
        flag.mScope = scope;
        auto startpos = mLexer.GetCurPos();
        FrontFlag(flag);
        // 2.解析类型
        Type(type);
        // 3.解析名称
        auto tok = mLexer.GetNext();
        if (tok.kind != TkKind::Identifier)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少名称！");
        }
        auto name = mPool.RefString(tok.text);

        // 4、解析变量声明
        MemberVar *decl = nullptr;
        auto decref = GenMember(decl);
        decl->mDecl.mFlag = flag;
        decl->mDecl.mType = type;
        decl->mDecl.mName = name;
        decl->mDecl.mStartPos = startpos;

        Variable(decl->mDecl);

        return decref;
    }

    AstMemberRef Parser::MemberUnionItem(ScopeEnum &defaultScope)
    {
        MemberUnion *decl = nullptr;
        auto decref = GenMember(decl);

        auto tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenBrace)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号：'{'！");
        }
        while (true)
        {
            tok = mLexer.GetNext();
            // 先查看是否是 union、struct 成员
            if (tok.kind == TkKind::KeyWord)
            {
                switch (tok.code.key)
                {
                case KeyEnum::Union:
                    decl->mItems.push_back(MemberUnionItem(defaultScope));
                    continue;
                case KeyEnum::Struct:
                    decl->mItems.push_back(MemberStructItem(defaultScope));
                    continue;
                default:
                    mLexer.BackTok(tok);
                    // 普通成员
                    decl->mItems.push_back(MemberVarItem(defaultScope));
                    continue;
                };
            }
            // '}'
            if (tok.kind == TkKind::Seperator)
            {
                if (tok.code.sp == SpEnum::CloseBrace)
                    break;
                goto _Error_;
            }
            // 普通成员
            decl->mItems.push_back(MemberVarItem(defaultScope));
        }

        return decref;

    _Error_:
    {
        Error("    line:  %d\n", tok.pos.mLine);
        Error("    tok:  %s\n", tok.text.c_str());
        throw ErrorExpception("缺少符号：'}'！");
    }
        return AstMemberRef();
    }

    AstMemberRef Parser::MemberStructItem(ScopeEnum &defaultScope)
    {
        MemberStruct *decl = nullptr;
        auto decref = GenMember(decl);

        auto tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenBrace)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号：'{'！");
        }
        while (true)
        {
            tok = mLexer.GetNext();
            // 先查看是否是 union、struct 成员
            if (tok.kind == TkKind::KeyWord)
            {
                switch (tok.code.key)
                {
                case KeyEnum::Union:
                    decl->mItems.push_back(MemberUnionItem(defaultScope));
                    continue;
                case KeyEnum::Struct:
                    decl->mItems.push_back(MemberStructItem(defaultScope));
                    continue;
                default:
                    mLexer.BackTok(tok);
                    // 普通成员
                    decl->mItems.push_back(MemberVarItem(defaultScope));
                    continue;
                };
            }
            // '}'
            if (tok.kind == TkKind::Seperator)
            {
                if (tok.code.sp == SpEnum::CloseBrace)
                    break;
                goto _Error_;
            }
            // 普通成员
            decl->mItems.push_back(MemberVarItem(defaultScope));
        }

        return decref;

    _Error_:
    {
        Error("    line:  %d\n", tok.pos.mLine);
        Error("    tok:  %s\n", tok.text.c_str());
        throw ErrorExpception("缺少符号：'}'！");
    }
        return AstMemberRef();
    }

    // 语法：struct name:base{decl*};
    AstDeclRef Parser::Struct(ScopeEnum scope)
    {
        StructDecl *decl = nullptr;
        auto declref = GenDecl(decl);
        auto tok = mLexer.GetNext();
        // 解析名称
        if (tok.kind != TkKind::Identifier)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少名称！");
        }
        decl->mName = mPool.RefString(tok.text);

        tok = mLexer.GetNext();
        // 只是声明
        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::SemiColon)
        {
            decl->mIsDecl = true;
            return declref;
        }

        // 查看是否有父类
        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Colon)
        {
            while (true)
            {
                tok = mLexer.GetNext();
                if (tok.kind != TkKind::Identifier)
                {
                    Error("    line:  %d\n", tok.pos.mLine);
                    Error("    tok:  %s\n", tok.text.c_str());
                    throw ErrorExpception("缺少名称！");
                }
                decl->mParent.push_back(mPool.RefString(tok.text));
                tok = mLexer.GetNext();
                if (tok.kind == TkKind::Operaor && tok.code.op == OpEnum::Dot)
                    continue;
                break;
            }
        }
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenBrace)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号 '{' ！");
        }
        // 解析成员
        while (true)
        {
            tok = mLexer.GetNext();
            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseBrace)
                break;
            mLexer.BackTok(tok);
            decl->mMembers.push_back(MemberItem(decl->mDefaultScope));
        }
        return declref;
    }
    // 静态函数语法：entrust name =  (paramlsit) rettype ;
    // 对象函数语法：entrust name = *(paramlsit) rettype ;
    AstDeclRef Parser::Entrust(ScopeEnum scope)
    {
        EntrustDecl *decl = nullptr;
        auto declref = GenDecl(decl);
        // 解析名称
        auto tok = mLexer.GetNext();
        if (tok.kind != TkKind::Identifier)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少名称！");
        }
        decl->mName = mPool.RefString(tok.text);
        // '='
        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Operaor || tok.code.op != OpEnum::Assign)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号 '=' ！");
        }
        // '*'
        tok = mLexer.GetNext();
        if (tok.kind == TkKind::Operaor && tok.code.op == OpEnum::Multiply)
        {
            decl->mIsObj = true;
            tok = mLexer.GetNext();
        }
        // '('
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenParen)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号 '(' ！");
        }
        // 解析参数列表
        ParamList(decl->mParams);

        // 解析返回值
        Type(decl->mReturn);
        // ';'
        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::SemiColon)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号 ';' ！");
        }
        return declref;
    }
    // 语法：interface name [base,base2]{decl*};
    AstDeclRef Parser::Interface(ScopeEnum scope)
    {
        InterfaceDecl *decl = nullptr;
        auto declref = GenDecl(decl);
        auto tok = mLexer.GetNext();
        // 解析名称
        if (tok.kind != TkKind::Identifier)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少名称！");
        }
        decl->mName = mPool.RefString(tok.text);

        tok = mLexer.GetNext();
        // 只是声明
        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::SemiColon)
        {
            decl->mIsDecl = true;
            return declref;
        }
        // 查看是否有接口
        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::OpenBracket)
        {
            while (true)
            {
                // 项解析接口
                auto &item = decl->mInterfaces.emplace_back();
                while (true)
                {
                    tok = mLexer.GetNext();
                    if (tok.kind != TkKind::Identifier)
                    {
                        Error("    line:  %d\n", tok.pos.mLine);
                        Error("    tok:  %s\n", tok.text.c_str());
                        throw ErrorExpception("缺少名称！");
                    }
                    item.mName.push_back(mPool.RefString(tok.text));
                    tok = mLexer.GetNext();
                    if (tok.kind == TkKind::Operaor && tok.code.op == OpEnum::Dot)
                        continue;
                    break;
                }
                // ','
                if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Comma)
                    continue;
                // ']'
                if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseBracket)
                    break;
                // 错误
                Error("    line:  %d\n", tok.pos.mLine);
                Error("    tok:  %s\n", tok.text.c_str());
                throw ErrorExpception("缺少符号 ']'！");
            }
            tok = mLexer.GetNext();
        }
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenBrace)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号 '{' ！");
        }
        // 解析成员
        while (true)
        {
            tok = mLexer.GetNext();
            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseBrace)
                break;
            mLexer.BackTok(tok);
            auto &item = decl->mMembers.emplace_back();
            InterfaceItem(item);
            // decl->mMembers.push_back(MemberItem(decl->mDefaultScope));
        }
        return declref;
    }
    void Parser::InterfaceItem(FunctionDecl &func)
    {
        func.mStartPos = mLexer.GetCurPos();
        FrontFlag(func.mFlag);
        func.mFlag.mScope = ScopeEnum::Public;
        // 解析类型
        Type(func.mRetType);
        // 解析名称
        auto tok = mLexer.GetNext();
        if (tok.kind != TkKind::Identifier)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少名称！");
        }
        func.mName = mPool.RefString(tok.text);

        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenParen)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号 '(' ！");
        }
        // 解析参数列表
        ParamList(func.mParams);

        tok = mLexer.GetNext();
        // ';'
        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::SemiColon)
        {
            func.mIsDecl = true;
            return;
        }
        Error("    line:  %d\n", tok.pos.mLine);
        Error("    tok:  %s\n", tok.text.c_str());
        throw ErrorExpception("缺少符号 ';' ！");
    }
    // 语法：class name:base [interface,interface2]{decl*};
    AstDeclRef Parser::Class(ScopeEnum scope)
    {
        ClassDecl *decl = nullptr;
        auto declref = GenDecl(decl);
        auto tok = mLexer.GetNext();
        // 解析名称
        if (tok.kind != TkKind::Identifier)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少名称！");
        }
        decl->mName = mPool.RefString(tok.text);

        tok = mLexer.GetNext();
        // 只是声明
        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::SemiColon)
        {
            decl->mIsDecl = true;
            return declref;
        }

        // 查看是否有父类
        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Colon)
        {
            while (true)
            {
                tok = mLexer.GetNext();
                if (tok.kind != TkKind::Identifier)
                {
                    Error("    line:  %d\n", tok.pos.mLine);
                    Error("    tok:  %s\n", tok.text.c_str());
                    throw ErrorExpception("缺少名称！");
                }
                decl->mParent.push_back(mPool.RefString(tok.text));
                tok = mLexer.GetNext();
                if (tok.kind == TkKind::Operaor && tok.code.op == OpEnum::Dot)
                    continue;
                break;
            }
        }
        // 查看是否有接口
        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::OpenBracket)
        {
            while (true)
            {
                // 项解析接口
                auto &item = decl->mInterfaces.emplace_back();
                while (true)
                {
                    tok = mLexer.GetNext();
                    if (tok.kind != TkKind::Identifier)
                    {
                        Error("    line:  %d\n", tok.pos.mLine);
                        Error("    tok:  %s\n", tok.text.c_str());
                        throw ErrorExpception("缺少名称！");
                    }
                    item.mName.push_back(mPool.RefString(tok.text));
                    tok = mLexer.GetNext();
                    if (tok.kind == TkKind::Operaor && tok.code.op == OpEnum::Dot)
                        continue;
                    break;
                }
                // ','
                if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Comma)
                    continue;
                // ']'
                if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseBracket)
                    break;
                // 错误
                Error("    line:  %d\n", tok.pos.mLine);
                Error("    tok:  %s\n", tok.text.c_str());
                throw ErrorExpception("缺少符号 ']'！");
            }
            tok = mLexer.GetNext();
        }
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenBrace)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号 '{' ！");
        }
        // 解析成员
        while (true)
        {
            tok = mLexer.GetNext();
            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseBrace)
                break;
            mLexer.BackTok(tok);
            decl->mMembers.push_back(MemberItem(decl->mDefaultScope));
        }
        return declref;
    }

}
