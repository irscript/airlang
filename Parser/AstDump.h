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
    };
}

#endif //!__ASTDUMP__H__