#ifndef __ASTDUMP__H__
#define __ASTDUMP__H__

// 打印语法分析树
#include <Utils/FileUnit.h>
namespace air
{
    struct AstDumper
    {
        AstDumper(FileUnit &unit) { Dump(unit); }

        static const char *mTab;

    private:
        void Dump(FileUnit &unit);
        void Import(ImportItems &imp);
        // 打印类型
        void Type(AstType &type, const std::string &szTab);
        // 打印声明
        void DeclList(std::list<AstDeclRef> &list);
        void VarDecl(AstDeclRef &decl, const std::string &szTab);
        void FuncDecl(AstDeclRef &decl, const std::string &szTab);
        void EnumDexl(AstDeclRef &decl, const std::string &szTab);
        void StructDecl(AstDeclRef &decl, const std::string &szTab);
        void InterfaceDecl(AstDeclRef &decl, const std::string &szTab);
        void ClassDecl(AstDeclRef &decl, const std::string &szTab);
        // 打印表达式
        void Expression(AstExpRef &exp, const std::string &szTab);
        void BinaryExpressio(AstExpRef &exp, const std::string &szTab);
        void UnaryExpressio(AstExpRef &exp, const std::string &szTab);
        void TernaryExpressio(AstExpRef &exp, const std::string &szTab);
        void DotExpressio(AstExpRef &exp, const std::string &szTab);
        void IdentExpressio(AstExpRef &exp, const std::string &szTab);
        void IntExpressio(AstExpRef &exp, const std::string &szTab);
        void UintExpressio(AstExpRef &exp, const std::string &szTab);
        void FloatExpressio(AstExpRef &exp, const std::string &szTab);
        void CharExpressio(AstExpRef &exp, const std::string &szTab);
        void CStringExpressio(AstExpRef &exp, const std::string &szTab);
        void NullExpressio(AstExpRef &exp, const std::string &szTab);
        void BoolExpressio(AstExpRef &exp, const std::string &szTab);
        void NewExpressio(AstExpRef &exp, const std::string &szTab);
        void FunCallExpressio(AstExpRef &exp, const std::string &szTab);
        void CastExpressio(AstExpRef &exp, const std::string &szTab);
        void ThisExpressio(AstExpRef &exp, const std::string &szTab);
        void SuperExpressio(AstExpRef &exp, const std::string &szTab);
        void ParenExpressio(AstExpRef &exp, const std::string &szTab);
        void ArrayExpressio(AstExpRef &exp, const std::string &szTab);
        void RangExpressio(AstExpRef &exp, const std::string &szTab);
        void BlockExpressio(AstExpRef &exp, const std::string &szTab);
        // 打印语句
        void Statament(AstStmRef &stm, const std::string &szTab);
        void BlockStatament(AstStmRef &stm, const std::string &szTab);
        void VarStatament(AstStmRef &stm, const std::string &szTab);
        void ExpStatament(AstStmRef &stm, const std::string &szTab);
        void IfStatament(AstStmRef &stm, const std::string &szTab);
        void ElsiifStatament(AstStmRef &stm, const std::string &szTab);
        void ElseStatament(AstStmRef &stm, const std::string &szTab);
        void SwitchStatament(AstStmRef &stm, const std::string &szTab);
        void CaseStatament(AstStmRef &stm, const std::string &szTab);
        void DefaultStatament(AstStmRef &stm, const std::string &szTab);
        void ForStatament(AstStmRef &stm, const std::string &szTab);
        void ForeachStatament(AstStmRef &stm, const std::string &szTab);
        void WhileStatament(AstStmRef &stm, const std::string &szTab);
        void DoWhileStatament(AstStmRef &stm, const std::string &szTab);
        void TryStatament(AstStmRef &stm, const std::string &szTab);
        void CatchStatament(AstStmRef &stm, const std::string &szTab);
        void FinallyStatament(AstStmRef &stm, const std::string &szTab);
        void LableStatament(AstStmRef &stm, const std::string &szTab);
        void GotoStatament(AstStmRef &stm, const std::string &szTab);
        void BreakStatament(AstStmRef &stm, const std::string &szTab);
        void ContinueStatament(AstStmRef &stm, const std::string &szTab);
        void ReturnStatament(AstStmRef &stm, const std::string &szTab);
        };
}

#endif //!__ASTDUMP__H__