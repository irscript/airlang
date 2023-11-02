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
        union
        {
            uint32_t mState; // 编译状态
            struct
            {
                uint32_t mIsError : 1;    // 编译是否错误
                uint32_t mIsSymbol : 1;   // 是否建立符号表
                uint32_t mIsSemantic : 1; // 是否语义分析
            };
        };

        FileUnit() : mState(0) {}

        StringRef mFile;      // 文件名称
        StringRef mPackage;   // 包名
        ImportItems mImports; // 导入项

        std::list<AstDeclRef> mAstTree; // 语法抽象树

        AstSymbolTable mSymTable; // 符号表

        // 查找符号
        bool FindSymbol(StringRef sym, AstSymbol *&def)
        {
            auto res = mSymTable.find(sym);
            if (res == mSymTable.end())
                return false;
            def = &res->second;
            return true;
        }
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