#ifndef __FILEUNIT__H__
#define __FILEUNIT__H__
#include <Utils/Pch.h>
#include <Utils/StringPool.h>
#include <Parser/AstNode.h>
namespace air
{

    // 文件单元
    struct FileUnit
    {
        StringRef mFile;      // 文件名称
        StringRef mPackage;   // 包名
        ImportItems mImports; // 导入项

        std::list<AstDeclRef> mAstTree; // 语法抽象树

        AstSymbolTable mSymTable; // 符号表
    };

    // 项目文件集

    struct FileCenter
    {
        std::map<StringRef, FileUnit> mFileUnits; // 文件单元映射集

        // 获取文件单元，没有就插入
        auto &GetFileUint(StringRef file)
        {
            auto res = mFileUnits.insert({file, FileUnit()});
            // 插入成功
            if (res.second == true)
                res.first->second.mFile = file;

            return res.first->second;
        }

        // 检查文件单元是否已经存在
        bool CheckFileUint(StringRef file) { return mFileUnits.find(file) != mFileUnits.end(); }
    };

}

#endif //!__FILEUNIT__H__