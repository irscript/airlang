#ifndef __SEMANTIC__H__
#define __SEMANTIC__H__
#include <Utils/FileUnit.h>
#include <Lexer/Scanner.h>
#include <Utils/ErrorExp.h>
#include <Utils/Project.h>
namespace air
{
    /*
    语义分析
    1、建立符号表
    2、类型分析：内存大小、对齐大小；
    3、变量分析：大小、表达式检查
    4、函数分析：参数列表检查，语句检查、表达式检查
    */

    // 建立符号表 Symbol，并进行类型计算
    struct Semanticer
    {
        FileCenter &mCenter; // 文件中心
        FileUnit &mUnit;     // 文件单元
        StringPool &mPool;   // 字符串池

        std::map<StringRef, SysType> &mSysType; // 系统类型

        uint32_t mAddressSize; // 地址大小

        Semanticer(FileUnit &unit, FileCenter &center, StringPool &pool,
                   std::map<StringRef, SysType> &systype, uint32_t addressSize)
            : mCenter(center), mUnit(unit),
              mPool(pool), mSysType(systype), mAddressSize(addressSize) { Start(); }

    protected:
        // 符号表建立
        void Start();
        void Import();

        // 检查类型
        void CheckType(AstType &type);
        // 获取初始表达式的值
        bool GetExpValue(AstExpRef &exp);

        // 分析变量
        void Var(VariableDecl &var);
        // 分析函数
        void Fun(FunctionDecl &fun);
        // 分析枚举
        void Enum(EnumDecl &enm);
        // 分析委托
        void Entrust(EntrustDecl &ent);
        // 分析结构体
        void Struct(StructDecl &stc);
        // 分析类
        void Class(ClassDecl &cls);
    };

}
#endif //!__SEMANTIC__H__
