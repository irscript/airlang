#ifndef __ASTDUMP__H__
#define __ASTDUMP__H__

// 打印语法分析树
#include <Utils/FileUnit.h>
namespace air
{
    struct AstDumper
    {
        AstDumper(FileUnit &unit) { Dump(unit); }

    private:
        void Dump(FileUnit &unit);
        void Import(ImportItems &imp);
        // 打印声明
        void Decl(std::list<AstDeclRef> &list);

        void VarDecl(AstDeclRef &decl, const std::string &szTab);
        void EnumDexl(AstDeclRef &decl, const std::string &szTab);

        // 打印表达式
        void Expression(AstExpRef &exp, const std::string &szTab);
    };
}

#endif //!__ASTDUMP__H__