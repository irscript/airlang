#include "Parser.h"
#include <Utils/FileHelp.h>
#include <Utils/MapTable.h>

namespace air
{
    //---------------------------表达式------------------------------
    // 语法：{exp:exp,Exp:{}}
    // 语法：{exp,{}}
    AstExpRef Parser::BlockExpression()
    {
        BlockExp *exp = nullptr;
        auto expres = GenExp(exp);
        exp->mStartPos = mLexer.GetCurPos();
        AstExpRef exp1 = Expression();
        auto tok = mLexer.GetNext();
        // 语法：{exp:exp,Exp:{}}
        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Colon)
        {
            exp->mIndex = true;
            // 先生成第一个
            auto &item = exp->mItems.emplace_back();
            item.mID = exp1;
            item.mVal = Expression();
            tok = mLexer.GetNext();
            // 继续解析成员
            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Comma)
            {
                while (true)
                {
                    tok = mLexer.GetNext();
                    if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseBrace)
                        break;
                    mLexer.BackTok(tok);
                    auto &item = exp->mItems.emplace_back();
                    item.mID = Expression();
                    // '：'
                    tok = mLexer.GetNext();
                    if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::Colon)
                    {
                        // 错误
                        Error("    line:  %d\n", tok.pos.mLine);
                        Error("    tok:  %s\n", tok.text.c_str());
                        throw ErrorExpception("缺少符号 ':' ！");
                    }
                    item.mVal = Expression();
                    // 查看是否继续
                    tok = mLexer.GetNext();
                    if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Comma)
                        continue;
                    if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseBrace)
                        break;
                    // 错误
                    goto _Error_;
                }
            }
            else if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::CloseBrace)
                goto _Error_;

            return expres;
        } // 语法：{exp,{}}
        else
        {
            exp->mIndex = false;
            // 先生成第一个
            auto &item = exp->mItems.emplace_back();
            item.mVal = exp1;
            // 继续解析成员
            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Comma)
            {
                while (true)
                {
                    tok = mLexer.GetNext();
                    if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseBrace)
                        break;
                    mLexer.BackTok(tok);
                    auto &item = exp->mItems.emplace_back();
                    item.mVal = Expression();
                    tok = mLexer.GetNext();
                    if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::Comma)
                        continue;
                    if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseBrace)
                        break;
                    // 错误
                    goto _Error_;
                }
            }
            else if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::CloseBrace)
                goto _Error_;

            return expres;
        }
    _Error_:
    {
        // 错误
        Error("    line:  %d\n", tok.pos.mLine);
        Error("    tok:  %s\n", tok.text.c_str());
        throw ErrorExpception("缺少符号 '}' ！");
    }
        return expres;
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
                        Error("    line:  %d\n", tok.pos.mLine);
                        Error("    tok:  %s\n", tok.text.c_str());
                        throw ErrorExpception("缺少符号'<' ！");
                    }

                    Type(exp->mType);

                    if (tok.kind != TkKind::Operaor || tok.code.op != OpEnum::GT)
                    {
                        Error("    line:  %d\n", tok.pos.mLine);
                        Error("    tok:  %s\n", tok.text.c_str());
                        throw ErrorExpception("缺少符号'>' ！");
                    }
                }
                // 解析表达式 (exp)
                {
                    tok = mLexer.GetNext();
                    if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenParen)
                    {
                        Error("    line:  %d\n", tok.pos.mLine);
                        Error("    tok:  %s\n", tok.text.c_str());
                        throw ErrorExpception("缺少符号'(' ！");
                    }

                    exp->mExp = Expression();

                    tok = mLexer.GetNext();
                    if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::CloseParen)
                    {
                        Error("    line:  %d\n", tok.pos.mLine);
                        Error("    tok:  %s\n", tok.text.c_str());
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
                auto expres = GenExp(exp, tok.code.key == KeyEnum::This ? ExpKind::This : ExpKind::Super);
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
                    Error("    line:  %d\n", tok.pos.mLine);
                    Error("    tok:  %s\n", tok.text.c_str());
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
            if (tok.GetIntVal(val, mPool, type))
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
            tok.GetFltVal(exp->mValue, mPool, exp->mType);
            return expres;
        }
        break;
        }
    _Error_:
    {
        Error("    line:  %d\n", tok.pos.mLine);
        Error("    tok:  %s\n", tok.text.c_str());
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
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
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
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
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
                    Error("    line:  %d\n", tok.pos.mLine);
                    Error("    tok:  %s\n", tok.text.c_str());
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
            (tok.code.op == OpEnum::Plus ||   // +
             tok.code.op == OpEnum::Plus2 ||  // ++
             tok.code.op == OpEnum::Minus ||  // -
             tok.code.op == OpEnum::Minus2 || // --
             tok.code.op == OpEnum::BitAnd || // & 取址
             tok.code.op == OpEnum::Multiply  // * 取值
             ))
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
            mLexer.BackTok(tok);
            while (true)
            {
                // 检查表达式类别
                switch (basic->GetKind())
                {
                case ExpKind::Identity:
                case ExpKind::Array:
                case ExpKind::FunCall:
                {
                    if (basic->GetKind() == ExpKind::FunCall)
                        dot->mFunCall = true;
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
                    Error("    line:  %d\n", tok.pos.mLine);
                    Error("    tok:  %s\n", tok.text.c_str());
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
        int32_t priority = INT32_MAX;
        // 不是操作符
        if (tok.kind != TkKind::Operaor)
        {
            mLexer.BackTok(tok);
            return leftExp;
        }
        // 查找二元操作符
        priority = MapTable::FindOpPriority(tok.code.op);
        if (priority == INT32_MAX)
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
            Error("    line:  %d\n", tok.pos.mLine);
            Error("    tok:  %s\n", tok.text.c_str());
            throw ErrorExpception("缺少符号: ':' ！");
        }
        ter->mSelect2 = Expression();
        ter->mEndPos = ter->mSelect2->mEndPos;
        return exp;
    }

}
