#include "AstDump.h"
#include <Utils/Loger.h>
namespace air
{
    void AstDumper::Dump(FileUnit &unit)
    {
        Print("\ndump file: %s\n\n", unit.mFile->c_str());
        // 打印包名
        Info("package %s;\n\n", unit.mPackage->c_str());
        // 打印依赖项
        Import(unit.mImports);
        // 打印声明
        Decl(unit.mAstTree);
    }
    void AstDumper::Import(ImportItems &imp)
    {
        for (auto item : imp.mItems)
        {
            Info("package %s = %s\n", item.first->c_str(), item.second->c_str());
        }
        Print("\n");
    }
    void AstDumper::Decl(std::list<AstDeclRef> &list)
    {
        std::string szTab;
        for (auto item : list)
        {
            switch (item->GetKind())
            {
            case DeclKind::Var:
                VarDecl(item, szTab);
                break;

            default:
                break;
            }
        }
    }
    void AstDumper::VarDecl(AstDeclRef &var, const std::string &szTab)
    {
    }
}
