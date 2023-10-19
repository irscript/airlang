#include "AstDump.h"
#include <Utils/Loger.h>
namespace air
{
    const char *AstDumper::mTab = "    ";
    void AstDumper::Dump(FileUnit &unit)
    {
        Print("\ndump file: %s\n\n", unit.mFile->c_str());
        // 打印包名
        Info("package %s;\n\n", unit.mPackage->c_str());
        // 打印依赖项
        Import(unit.mImports);
        // 打印声明
        DeclList(unit.mAstTree);
    }
    void AstDumper::Import(ImportItems &imp)
    {
        for (auto item : imp.mItems)
        {
            Info("import %s = %s;\n", item.first->c_str(), item.second->c_str());
        }
        Print("\n");
    }
    void AstDumper::Type(AstType &type, const std::string &szTab)
    {
        std::string szTmp, szTmp2;
        Info("%s type:\n", szTab.c_str());
        szTmp = szTab + mTab;
        if (type.mConst)
            Print("%s flag: const\n", szTmp.c_str());

        Print("%s name:\n", szTmp.c_str());
        szTmp2 = szTmp + mTab;
        int i = 0;
        for (auto item : type.mName)
        {
            Print("%s %s\n", szTmp2.c_str(), item->c_str());
            ++i;
            if (i != type.mName.size())
                Print("%s       .\n", szTmp2.c_str());
        }
        if (type.mArrDynCol != 0)
        {
            if (type.mArrBlk)
            {
                Print("%s array:[", szTmp.c_str());
                for (auto item = 0; item < type.mArrDynCol - 1; ++item)
                {
                    Print(" ,");
                }
                Print(" ]");
            }
            else
            {
                Print("%s array:", szTmp.c_str());
                for (auto item = 0; item < type.mArrDynCol; ++item)
                {
                    Print(" []");
                }
            }
        }
        if (type.mReference)
            Print(" &");
        Print("\n");
    }
    void AstDumper::DeclList(std::list<AstDeclRef> &list)
    {
        std::string szTab;
        for (auto item : list)
        {
            switch (item->GetKind())
            {
            case DeclKind::Var:
                VarDecl(item, szTab);
                break;
            case DeclKind::Func:
                FuncDecl(item, szTab);
                break;

            default:
                break;
            }
        }
    }
    void AstDumper::VarDecl(AstDeclRef &var, const std::string &szTab)
    {
        std::string szTmp, szTmp2;
        VariableDecl &dec = *(VariableDecl *)(&*var);

        Info("%s var_decl_start\n", szTab.c_str());
        szTmp = szTab + mTab;
        // 打印标记
        if (dec.mFlag.mStatic)
            Print("%s flag: static\n", szTmp.c_str());
        // 类型
        Type(dec.mType, szTmp);
        // 名称
        Print("%s name: %s\n", szTmp.c_str(), dec.mName->c_str());
        // 静态数组
        if (dec.mArrCol.empty() == false)
        {
            Print("%s array:[\n", szTmp.c_str());
            szTmp2 = szTmp + mTab;
            int i = 0;
            for (auto item : dec.mArrCol)
            {
                Expression(item, szTmp2);
                ++i;
                if (i != dec.mArrCol.size())
                    Print("%s       ,\n", szTmp.c_str());
            }
            Print("%s       ]\n", szTmp.c_str());
        }
        // 初始值表达式
        Expression(dec.mInitExp, szTmp);
        Info("%s var_decl_end\n", szTab.c_str());

        for (auto vitem : dec.mVarItems)
            VarDecl(vitem, szTab);
    }
    void AstDumper::FuncDecl(AstDeclRef &decl, const std::string &szTab)
    {
    }
    void AstDumper::EnumDexl(AstDeclRef &decl, const std::string &szTab)
    {
    }
    void AstDumper::StructDecl(AstDeclRef &decl, const std::string &szTab)
    {
    }
    void AstDumper::InterfaceDecl(AstDeclRef &decl, const std::string &szTab)
    {
    }
    void AstDumper::ClassDecl(AstDeclRef &decl, const std::string &szTab)
    {
    }

    void AstDumper::Expression(AstExpRef &exp, const std::string &szTab)
    {
        if (exp.get() == nullptr)
            return;
        std::string szTmp, szTmp2;
        Info("%s exp:\n", szTab.c_str());
        szTmp = szTab + mTab;
        switch (exp->GetKind())
        {
        case ExpKind::Unary:
            UnaryExpressio(exp, szTmp);
            break;
        case ExpKind::Binary:
            BinaryExpressio(exp, szTmp);
            break;
        case ExpKind::Ternary:
            TernaryExpressio(exp, szTmp);
            break;

        case ExpKind::Identity:
            IdentExpressio(exp, szTmp);
            break;

        case ExpKind::Int:
            IntExpressio(exp, szTmp);
            break;
        case ExpKind::Uint:
            UintExpressio(exp, szTmp);
            break;
        case ExpKind::Float:
            FloatExpressio(exp, szTmp);
            break;
        case ExpKind::Char:
            CharExpressio(exp, szTmp);
            break;
        case ExpKind::CString:
            CStringExpressio(exp, szTmp);
            break;
        case ExpKind::Null:
            NullExpressio(exp, szTmp);
            break;
        case ExpKind::Bool:
            BoolExpressio(exp, szTmp);
            break;

        case ExpKind::New:
            NewExpressio(exp, szTmp);
            break;
        case ExpKind::FunCall:
            FunCallExpressio(exp, szTmp);
            break;
        case ExpKind::Cast:
            CastExpressio(exp, szTmp);
            break;
        case ExpKind::Super:
            SuperExpressio(exp, szTmp);
            break;
        case ExpKind::This:
            ThisExpressio(exp, szTmp);
            break;
        case ExpKind::Dot:
            DotExpressio(exp, szTmp);
            break;
        case ExpKind::Paren:
            ParenExpressio(exp, szTmp);
            break;
        case ExpKind::Array:
            ArrayExpressio(exp, szTmp);
            break;
        case ExpKind::Rang:
            RangExpressio(exp, szTmp);
            break;
        case ExpKind::Block:
            BlockExpressio(exp, szTmp);
            break;
        default:
            Error("未知表达式！");
            break;
        }
    }
    void AstDumper::BinaryExpressio(AstExpRef &exp, const std::string &szTab)
    {
        std::string szTmp, szTmp2;
        Info("%s Binary:\n", szTab.c_str());
        szTmp = szTab + mTab;
        szTmp2 = szTmp + mTab;
        BinaryExp &expref = *(BinaryExp *)(exp.get());
        Print("%s left:\n", szTmp.c_str());
        Expression(expref.mLeft, szTmp2);

        Print("%s op:", szTmp.c_str());
        switch (expref.mOp)
        {
        case OpEnum::RightShift:
            Info("<<\n");
            break;
        case OpEnum::LeftShift:
            Info(">>\n");
            break;
        case OpEnum::LogicalRightShift:
            Info("<<<\n");
            break;

        case OpEnum::BitAnd:
            Info("&\n");
            break;
        case OpEnum::BitXOr:
            Info("^\n");
            break;
        case OpEnum::BitOr:
            Info("|\n");
            break;

        case OpEnum::And:
            Info("&&\n");
            break;
        case OpEnum::Or:
            Info("||\n");
            break;

        case OpEnum::Plus:
            Info("+\n");
            break;
        case OpEnum::Minus:
            Info("-\n");
            break;
        case OpEnum::Multiply:
            Info("*\n");
            break;
        case OpEnum::Divide:
            Info("/\n");
            break;
        case OpEnum::Modulus:
            Info("%%\n");
            break;

        case OpEnum::Assign:
            Info("=\n");
            break;
        case OpEnum::PlusAssign:
            Info("+=\n");
            break;
        case OpEnum::MinusAssign:
            Info("-=\n");
            break;
        case OpEnum::MulAssign:
            Info("*=\n");
            break;
        case OpEnum::DivAssign:
            Info("/=\n");
            break;
        case OpEnum::ModAssign:
            Info("%%=\n");
            break;

        case OpEnum::RightShiftAssign:
            Info("<<=\n");
            break;
        case OpEnum::LeftShiftAssign:
            Info(">>=\n");
            break;
        case OpEnum::LogicalRightShiftAssign:
            Info("<<<=\n");
            break;

        case OpEnum::EQ:
            Info("==\n");
        case OpEnum::NE:
            Info("!=\n");
        case OpEnum::GT:
            Info(">\n");
        case OpEnum::GE:
            Info(">=\n");
        case OpEnum::LT:
            Info("<\n");
        case OpEnum::LE:
            Info("<=\n");
            break;
        default:
            Error("未知二元操作符：%d !\n", expref.mOp);
            break;
        }

        Info("%s right:\n", szTmp.c_str());
        Expression(expref.mRight, szTmp2);
    }
    void AstDumper::UnaryExpressio(AstExpRef &exp, const std::string &szTab)
    {
        std::string szTmp, szTmp2;
        Info("%s Unary: ", szTab.c_str());
        szTmp = szTab + mTab;
        auto &expref = *(UnaryExp *)(exp.get());
        //Print(" %s\n", expref.mID->c_str());
    }
    void AstDumper::TernaryExpressio(AstExpRef &exp, const std::string &szTab)
    {
        std::string szTmp, szTmp2;
        Info("%s Ternary:\n", szTab.c_str());
        szTmp = szTab + mTab;
        auto &expref = *(TernaryExp *)(exp.get());
    }
    void AstDumper::DotExpressio(AstExpRef &exp, const std::string &szTab)
    {
        std::string szTmp, szTmp2;
        Info("%s Dot:\n", szTab.c_str());
        szTmp = szTab + mTab;
        auto &expref = *(DotCallExp *)(exp.get());
    }

    void AstDumper::IdentExpressio(AstExpRef &exp, const std::string &szTab)
    {
        std::string szTmp, szTmp2;
        Info("%s ID:\n", szTab.c_str());
        szTmp = szTab + mTab;
        auto &expref = *(IdentExp *)(exp.get());
    }

    void AstDumper::IntExpressio(AstExpRef &exp, const std::string &szTab)
    {
        std::string szTmp, szTmp2;
        Info("%s Int:\n", szTab.c_str());
        szTmp = szTab + mTab;
        auto &expref = *(IntExp *)(exp.get());
    }

    void AstDumper::UintExpressio(AstExpRef &exp, const std::string &szTab)
    {
        std::string szTmp, szTmp2;
        Info("%s Uint:\n", szTab.c_str());
        szTmp = szTab + mTab;
        auto &expref = *(UintExp *)(exp.get());
    }

    void AstDumper::FloatExpressio(AstExpRef &exp, const std::string &szTab)
    {
        std::string szTmp, szTmp2;
        Info("%s Float:\n", szTab.c_str());
        szTmp = szTab + mTab;
        auto &expref = *(FloatExp *)(exp.get());
    }

    void AstDumper::CharExpressio(AstExpRef &exp, const std::string &szTab)
    {
        std::string szTmp, szTmp2;
        Info("%s Char:\n", szTab.c_str());
        szTmp = szTab + mTab;
        auto &expref = *(CharExp *)(exp.get());
    }

    void AstDumper::CStringExpressio(AstExpRef &exp, const std::string &szTab)
    {
        std::string szTmp, szTmp2;
        Info("%s CString:\n", szTab.c_str());
        szTmp = szTab + mTab;
        auto &expref = *(IdentExp *)(exp.get());
    }

    void AstDumper::NullExpressio(AstExpRef &exp, const std::string &szTab)
    {
        std::string szTmp, szTmp2;
        Info("%s nullptr\n", szTab.c_str());
    }

    void AstDumper::BoolExpressio(AstExpRef &exp, const std::string &szTab)
    {
        BoolExp &expref = *(BoolExp *)(exp.get());
        Info("%s %s\n", szTab.c_str(), expref.mVal ? "true" : "false");
    }

    void AstDumper::NewExpressio(AstExpRef &exp, const std::string &szTab)
    {
        std::string szTmp, szTmp2;
        Info("%s New:\n", szTab.c_str());
        szTmp = szTab + mTab;
        auto &expref = *(NewExp *)(exp.get());
    }

    void AstDumper::FunCallExpressio(AstExpRef &exp, const std::string &szTab)
    {
        std::string szTmp, szTmp2;
        Info("%s FunCall:\n", szTab.c_str());
        szTmp = szTab + mTab;
        auto &expref = *(FunCallExp *)(exp.get());
    }

    void AstDumper::CastExpressio(AstExpRef &exp, const std::string &szTab)
    {
        std::string szTmp, szTmp2;
        Info("%s Cast:\n", szTab.c_str());
        szTmp = szTab + mTab;
        auto &expref = *(CastExp *)(exp.get());
    }

    void AstDumper::ThisExpressio(AstExpRef &exp, const std::string &szTab)
    {
        ThisSuperExp &expref = *(ThisSuperExp *)(exp.get());
        Info("%s %s\n", szTab.c_str(), "this");
    }

    void AstDumper::SuperExpressio(AstExpRef &exp, const std::string &szTab)
    {
        ThisSuperExp &expref = *(ThisSuperExp *)(exp.get());
        Info("%s %s\n", szTab.c_str(), "super");
    }

    void AstDumper::ParenExpressio(AstExpRef &exp, const std::string &szTab)
    {
        std::string szTmp, szTmp2;
        Info("%s Paren:\n", szTab.c_str());
        szTmp = szTab + mTab;
        auto &expref = *(ParenExp *)(exp.get());
        Expression(expref.mExp, szTmp);
    }

    void AstDumper::ArrayExpressio(AstExpRef &exp, const std::string &szTab)
    {
        std::string szTmp, szTmp2;
        Info("%s Array:\n", szTab.c_str());
        szTmp = szTab + mTab;
        auto &expref = *(IdentExp *)(exp.get());
    }

    void AstDumper::RangExpressio(AstExpRef &exp, const std::string &szTab)
    {
        std::string szTmp, szTmp2;
        Info("%s Rang:\n", szTab.c_str());
        szTmp = szTab + mTab;
        auto &expref = *(IdentExp *)(exp.get());
    }

    void AstDumper::BlockExpressio(AstExpRef &exp, const std::string &szTab)
    {
    }

    void AstDumper::Statament(AstStmRef &stm, const std::string &szTab)
    {
    }
}
