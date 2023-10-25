#include "Parser.h"
#include <Utils/FileHelp.h>
#include <Utils/MapTable.h>

namespace air
{
    
    //--------------------------语句----------------------------
    void Parser::BlockStatement(BlockStm &stm)
    {
        while (true)
        {
            auto tok = mLexer.GetNext();
            // 结束符 }
            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseBrace)
                break;
            // 无意义分号 ';'
            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::SemiColon)
                continue;
            // 新的语句块 '{'
            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::OpenBrace)
            {
                BlockStm *blkstm = nullptr;
                auto res = GenStm(blkstm,BlockKind::Normal);
                blkstm->mStartPos = tok.pos;
                BlockStatement(*blkstm);
                stm.mStatement.push_back(res);
                continue;
            }
            // 关键字
            if (tok.kind == TkKind::KeyWord)
            {
                switch (tok.code.key)
                {
                case KeyEnum::If:
                {
                    auto res = IfStatement();
                    res->mStartPos = tok.pos;
                    stm.mStatement.push_back(res);
                    continue;
                }
                break;
                case KeyEnum::Switch:
                {
                    auto res = SwitchStatement();
                    res->mStartPos = tok.pos;
                    stm.mStatement.push_back(res);
                    continue;
                }
                break;
                case KeyEnum::For:
                {
                    auto res = ForStatement();
                    res->mStartPos = tok.pos;
                    stm.mStatement.push_back(res);
                    continue;
                }
                break;
                case KeyEnum::While:
                {
                    auto res = WhileStatement();
                    res->mStartPos = tok.pos;
                    stm.mStatement.push_back(res);
                    continue;
                }
                break;
                case KeyEnum::Do:
                {
                    auto res = DoWhileStatement();
                    res->mStartPos = tok.pos;
                    stm.mStatement.push_back(res);
                    continue;
                }
                break;
                case KeyEnum::Try:
                {
                    auto res = TryStatement();
                    res->mStartPos = tok.pos;
                    stm.mStatement.push_back(res);
                    continue;
                }
                break;
                // goto lable;
                case KeyEnum::Goto:
                {
                    GotoStm *gotostm = nullptr;
                    auto res = GenStm(gotostm);
                    gotostm->mStartPos = tok.pos;

                    // 解析标签
                    tok = mLexer.GetNext();
                    if (tok.kind != TkKind::Identifier)
                    {
                        Error("    line:  %d\n", tok.pos.mLine);
                        Error("    tok:  %s\n", tok.text.c_str());
                        throw ErrorExpception("缺少标签名 ！");
                    }
                    gotostm->mLabel = mPool.RefString(tok.text);
                    // 确保 ';'
                    tok = mLexer.GetNext();
                    if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::SemiColon)
                    {
                        Error("    line:  %d\n", tok.pos.mLine);
                        Error("    tok:  %s\n", tok.text.c_str());
                        throw ErrorExpception("缺少符号: ';' ！");
                    }
                    gotostm->mEndPos = mLexer.GetCurPos();

                    stm.mStatement.push_back(res);
                    continue;
                }
                break;
                case KeyEnum::Continue:
                {
                    ContinueStm *stmptr = nullptr;
                    auto res = GenStm(stmptr);
                    stmptr->mStartPos = tok.pos;
                    // 确保 ';'
                    tok = mLexer.GetNext();
                    if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::SemiColon)
                    {
                        Error("    line:  %d\n", tok.pos.mLine);
                        Error("    tok:  %s\n", tok.text.c_str());
                        throw ErrorExpception("缺少符号: ';' ！");
                    }
                    stmptr->mEndPos = mLexer.GetCurPos();
                    stm.mStatement.push_back(res);
                    continue;
                }
                break;
                case KeyEnum::Break:
                {
                    BreakStm *stmptr = nullptr;
                    auto res = GenStm(stmptr);
                    stmptr->mStartPos = tok.pos;
                    // 确保 ';'
                    tok = mLexer.GetNext();
                    if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::SemiColon)
                    {
                        Error("    line:  %d\n", tok.pos.mLine);
                        Error("    tok:  %s\n", tok.text.c_str());
                        throw ErrorExpception("缺少符号: ';' ！");
                    }
                    stmptr->mEndPos = mLexer.GetCurPos();
                    stm.mStatement.push_back(res);
                    continue;
                }
                break;
                case KeyEnum::Return:
                {
                    ReturnStm *stmptr = nullptr;
                    auto res = GenStm(stmptr);
                    stmptr->mStartPos = tok.pos;
                    // 解析返回值表达式
                    stmptr->mVal = Expression();
                    // 确保 ';'
                    tok = mLexer.GetNext();
                    if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::SemiColon)
                    {
                        Error("    line:  %d\n", tok.pos.mLine);
                        Error("    tok:  %s\n", tok.text.c_str());
                        throw ErrorExpception("缺少符号: ';' ！");
                    }
                    stmptr->mEndPos = mLexer.GetCurPos();
                    stm.mStatement.push_back(res);
                    continue;
                }
                break;
                default:
                {
                    mLexer.BackTok(tok);
                    goto _Var_Exp_;
                }
                break;
                }
            }
            // 标签定义
            if (tok.kind == TkKind::Identifier)
            {

                auto tok2 = mLexer.GetNext();
                if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Colon)
                {
                    LabelStm *stmptr = nullptr;
                    stm.mStatement.push_back(GenStm(stmptr, mPool.RefString(tok.text)));
                    continue;
                }
                mLexer.BackTok(tok2);
                mLexer.BackTok(tok);
            }

        _Var_Exp_:
            // 变量定义语句、表达式语句
            stm.mStatement.push_back(VarStatement());
        }
        stm.mEndPos = mLexer.GetCurPos();
    }
    AstStmRef Parser::VarStatement()
    {
        // 解析前置标记
        AstFlag flag;
        // 解析类型
        AstType type;
        auto tok = mLexer.GetNext();
        if (tok.kind == TkKind::KeyWord)
        {
            switch (tok.code.key)
            {
            case KeyEnum::Static:
            {
                flag.mStatic = 1;
                goto _EnusureVarDef_;
            }
            break;
            case KeyEnum::Const:
            {
                mLexer.BackTok(tok);
                goto _EnusureVarDef_;
            }
            break;
            default:
            {
                mLexer.BackTok(tok);
                // 基本类型关键字
                if (KeyEnum::Void < tok.code.key && tok.code.key < KeyEnum::Nullptr)
                    goto _EnusureVarDef_;

                // 可能是表达式语句
                goto _MayBeExp_;
            }
            break;
            }
        }
        // 先看看是否表达式
        else if (tok.kind == TkKind::Identifier)
        {
            mLexer.BackTok(tok);
            auto exp = Expression();
            switch (exp->GetKind())
            {
            case ExpKind::Identity:
            {
                auto tok2 = mLexer.GetNext();
                // 逗号表达式
                if (tok2.kind == TkKind::Seperator && tok2.code.sp == SpEnum::Comma)
                {
                    ExpStm *expstmptr = nullptr;
                    auto stm = GenStm(expstmptr, exp);
                    while (true)
                    {
                        expstmptr->mExpItems.push_back(Expression());
                        tok = mLexer.GetNext();
                        // ,
                        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Comma)
                            continue;
                        // 确保 ；
                        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::SemiColon)
                            return stm;
                        // 错误
                        Error("    line:  %d\n", tok.pos.mLine);
                        Error("    tok:  %s\n", tok.text.c_str());
                        throw ErrorExpception("缺少符号: ';' ！");
                    }
                }
                // 确保 ；
                if (tok2.kind == TkKind::Seperator && tok2.code.sp == SpEnum::SemiColon)
                {
                    ExpStm *expstmptr = nullptr;
                    auto stm = GenStm(expstmptr, exp);
                    expstmptr->mEndPos = mLexer.GetCurPos();
                    tok = mLexer.GetNext();
                    return stm;
                }
                // 不是表达式，看看是不是变量定义
                mLexer.BackTok(tok);
                goto _EnusureVarDef_;
            }
            break;
            case ExpKind::Dot:
            {
                DotCallExp &dot = *(DotCallExp *)(exp.get());
                if (dot.mFunCall == true)
                {
                    ExpStm *expstmptr = nullptr;
                    auto stm = GenStm(expstmptr, exp);
                    expstmptr->mEndPos = mLexer.GetCurPos();
                    tok = mLexer.GetNext();
                    // 逗号表达式
                    if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Comma)
                    {
                        ExpStm *expstmptr = nullptr;
                        auto stm = GenStm(expstmptr, exp);
                        while (true)
                        {
                            expstmptr->mExpItems.push_back(Expression());
                            tok = mLexer.GetNext();
                            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Comma)
                                continue;
                            break;
                        }
                    }
                    // 确保 ；
                    if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::SemiColon)
                    {
                        // 错误
                        Error("    line:  %d\n", tok.pos.mLine);
                        Error("    tok:  %s\n", tok.text.c_str());
                        throw ErrorExpception("缺少符号 ';' ！");
                    }
                    return stm;
                }
                // 可能是表达式
                auto tok2 = mLexer.GetNext();
                // 逗号表达式
                if (tok2.kind == TkKind::Seperator && tok2.code.sp == SpEnum::Comma)
                {
                    ExpStm *expstmptr = nullptr;
                    auto stm = GenStm(expstmptr, exp);
                    while (true)
                    {
                        expstmptr->mExpItems.push_back(Expression());
                        tok = mLexer.GetNext();
                        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Comma)
                            continue;
                        // 确保 ；
                        if (tok2.kind == TkKind::Seperator && tok2.code.sp == SpEnum::SemiColon)
                            return stm;
                        // 错误
                        Error("    line:  %d\n", tok.pos.mLine);
                        Error("    tok:  %s\n", tok.text.c_str());
                        throw ErrorExpception("缺少符号: ';' ！");
                    }
                }
                // 确保 ；
                if (tok2.kind == TkKind::Seperator && tok2.code.sp == SpEnum::SemiColon)
                {
                    ExpStm *expstmptr = nullptr;
                    auto stm = GenStm(expstmptr, exp);
                    expstmptr->mEndPos = mLexer.GetCurPos();

                    return stm;
                }
                // 不是表达式，看看是不是变量定义
                mLexer.BackTok(tok);
                goto _EnusureVarDef_;
            }
            break;
            // 就是表达式
            default:
            {
                mLexer.BackTok(tok);
                goto _MayBeExp_;
            }
            }
        }
        // 可能是表达式语句
        else
        {
            mLexer.BackTok(tok);
        }
        // 可能是表达式语句
    _MayBeExp_:
    {
        auto exp = Expression();
        ExpStm *stmptr = nullptr;
        auto stm = GenStm(stmptr, exp);

        tok = mLexer.GetNext();
        // 逗号表达式
        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Comma)
        {
            while (true)
            {
                stmptr->mExpItems.push_back(Expression());
                tok = mLexer.GetNext();
                if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Comma)
                    continue;
                break;
            }
        }
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::SemiColon)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: ';' ！");
        }
        return stm;
    }

        // 确定是变量定义
    _EnusureVarDef_:
        Type(type);
        // 解析名称
        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Identifier)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少名称！");
        }
        VariableDecl *decl = nullptr;
        auto decref = GenDecl(decl);
        decl->mFlag = flag;
        decl->mType = type;
        decl->mName = mPool.RefString(tok.text);
        decl->mStartPos = tok.pos;

        VarStm *varstmptr = nullptr;
        auto stm = GenStm(varstmptr);

        Variable(*decl);

        return stm;
    }
    AstStmRef Parser::IfStatement()
    {
        // (
        auto tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenParen)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: '(' ！");
        }

        IfStm *stmptr = nullptr;
        auto stm = GenStm(stmptr);
        stmptr->mStartPos = tok.pos;

        // exp
        stmptr->mCondExp = Expression();
        // )
        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::CloseParen)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: ')' ！");
        }
        tok = mLexer.GetNext();
        // {
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenBrace)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: '{' ！");
        }
        // 语句体
        BlockStatement(stmptr->mBlock);

        // elsif 和 else 语句
        while (true)
        {
            tok = mLexer.GetNext();
            if (tok.kind == TkKind::KeyWord)
            {
                switch (tok.code.key)
                {
                case KeyEnum::Elsif:
                    stmptr->mElsifs.push_back(ElsifStatement());
                    continue;
                case KeyEnum::Else:
                    stmptr->mElse = ElseStatement();
                    goto _End_;
                    break;
                default:
                    mLexer.BackTok(tok);
                    goto _End_;
                    break;
                }
            }
        }
    _End_:
        return stm;
    }
    AstStmRef Parser::ElsifStatement()
    {
        auto tok = mLexer.GetNext();
        // )
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenParen)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: '(' ！");
        }

        ElsifStm *stmptr = nullptr;
        auto stm = GenStm(stmptr);
        stmptr->mStartPos = tok.pos;
        // exp
        stmptr->mCondExp = Expression();
        // )
        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::CloseParen)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: ')' ！");
        }
        // {
        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenBrace)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: '{' ！");
        }
        // 语句体
        BlockStatement(stmptr->mBlock);
        return stm;
    }
    AstStmRef Parser::ElseStatement()
    {
        ElseStm *stmptr = nullptr;
        auto stm = GenStm(stmptr);
        stmptr->mStartPos = mLexer.GetCurPos();

        // ｛
        auto tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenBrace)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: '{' ！");
        } // 语句体
        BlockStatement(stmptr->mBlock);
        return stm;
    }

    AstStmRef Parser::SwitchStatement()
    {
        auto tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenParen)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: '(' ！");
        }

        SwitchStm *stmptr = nullptr;
        auto stm = GenStm(stmptr);
        stmptr->mStartPos = tok.pos;
        stmptr->mCondExp = Expression();

        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::CloseParen)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: ')' ！");
        }
        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenBrace)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: '{' ！");
        }
        while (true)
        {
            tok = mLexer.GetNext();
            // 结束符 }
            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseBrace)
                break;

            if (tok.kind == TkKind::KeyWord)
            {
                switch (tok.code.key)
                {
                case KeyEnum::Case:
                    stmptr->mCases.push_back(CaseStatement());
                    continue;
                case KeyEnum::Default:
                {
                    stmptr->mDefault = DefaultStatement();
                    tok = mLexer.GetNext();
                    // 结束符 }
                    if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseBrace)
                        return stm;
                }
                break;
                }
            }
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: '}' ！");
        }
    _End_:
        return stm;
    }
    AstStmRef Parser::CaseStatement()
    {
        CaseStm *stmptr = nullptr;
        auto stm = GenStm(stmptr);
        // 解析表达式
        stmptr->mCondExp = Expression();
        auto tok = mLexer.GetNext();
        // 确保 :
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::Colon)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: ':' ！");
        }
        // 确保 ｛
        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenBrace)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: '{' ！");
        }
        // 解析语句体
        BlockStatement(stmptr->mBlock);
        return stm;
    }
    AstStmRef Parser::DefaultStatement()
    {
        DefaultStm *stmptr = nullptr;
        auto stm = GenStm(stmptr);
        auto tok = mLexer.GetNext();
        // 确保 :
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::Colon)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: ':' ！");
        }
        // 确保 ｛
        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenBrace)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: '{' ！");
        }
        BlockStatement(stmptr->mBlock);
        return stm;
    }

    AstStmRef Parser::ForStatement()
    {
        ForStm *stmptr = nullptr;
        auto stm = GenStm(stmptr);
        // 解析条件表达式
        auto tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenParen)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: '(' ！");
        }
        // 初始条件
        stmptr->mInitExp = VarStatement();
        // 判断条件
        stmptr->mCondExp = Expression();
        // ;
        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::SemiColon)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: ';' ！");
        }
        // 结束表达式
        stmptr->mValExp.push_back(Expression());
        tok = mLexer.GetNext();
        // 逗号表达式
        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Comma)
        {
            while (true)
            {
                stmptr->mValExp.push_back(Expression());
                tok = mLexer.GetNext();
                if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Comma)
                    continue;
                break;
            }
        }
        // 验证 ）
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::CloseParen)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: ')' ！");
        }
        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenBrace)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: '{' ！");
        }
        // 解析语句体
        BlockStatement(stmptr->mBlock);
        return stm;
    }
    AstStmRef Parser::WhileStatement()
    {
        WhileStm *stmptr = nullptr;
        auto stm = GenStm(stmptr);
        // 解析条件表达式
        auto tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenParen)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: '(' ！");
        }

        stmptr->mCondExp = Expression();

        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::CloseParen)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: ')' ！");
        }
        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenBrace)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: '{' ！");
        }
        // 解析语句体
        BlockStatement(stmptr->mBlock);
        return stm;
    }
    AstStmRef Parser::DoWhileStatement()
    {
        WhileStm *stmptr = nullptr;
        auto stm = GenStm(stmptr);
        auto tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenBrace)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: '{' ！");
        }
        // 解析语句体
        BlockStatement(stmptr->mBlock);
        tok = mLexer.GetNext();
        if (tok.kind != TkKind::KeyWord || tok.code.key != KeyEnum::While)
        {
        }
        // 解析条件表达式
        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenParen)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: '(' ！");
        }

        stmptr->mCondExp = Expression();

        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::CloseParen)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: ')' ！");
        }
        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::SemiColon)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: ';' ！");
        }
        return stm;
    }

    AstStmRef Parser::TryStatement()
    {
        TryStm *stmptr = nullptr;
        auto stm = GenStm(stmptr);
        // 确保 ｛
        auto tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenBrace)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: '{' ！");
        }
        // 解析语句体
        BlockStatement(stmptr->mBlock);

        // catch 和 finally 语句
        while (true)
        {
            tok = mLexer.GetNext();
            if (tok.kind == TkKind::KeyWord)
            {
                switch (tok.code.key)
                {
                case KeyEnum::Catch:
                    stmptr->mCatchs.push_back(CatchStatement());
                    continue;
                case KeyEnum::Finally:
                    stmptr->mFinally = FinallyStatement();
                    goto _End_;
                    break;
                default:
                    mLexer.BackTok(tok);
                    goto _End_;
                    break;
                }
            }
        }
    _End_:
        return stm;
    }
    AstStmRef Parser::CatchStatement()
    {
        CatchStm *stmptr = nullptr;
        auto stm = GenStm(stmptr);
        auto tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenParen)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: '(' ！");
        }
        // 解析 类型
        Type(stmptr->mVar.mType);
        // 解析 名称
        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Identifier)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少名称 ！");
        }
        stmptr->mVar.mName = mPool.RefString(tok.text);
        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::CloseParen)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: ')' ！");
        }
        // 确保 ｛
        tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenBrace)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: '{' ！");
        }
        // 解析语句体
        BlockStatement(stmptr->mBlock);
        return stm;
    }
    AstStmRef Parser::FinallyStatement()
    {
        FinallyStm *stmptr = nullptr;
        auto stm = GenStm(stmptr);
        // 确保 ｛
        auto tok = mLexer.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenBrace)
        {
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: '{' ！");
        }
        // 解析语句体
        BlockStatement(stmptr->mBlock);
        return stm;
    }
}
