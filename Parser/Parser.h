#ifndef __PARSER__H__
#define __PARSER__H__

#include <Utils/FileUnit.h>
#include <Lexer/Scanner.h>
#include <Utils/ErrorExp.h>
namespace air
{
    // 语法分析器
    struct Parser
    {
        Scanner mLexer;    // 词法分析器
        FileUnit &mUnit;   // 文件单元
        StringPool &mPool; // 字符串池

        ScopeEnum mScope; // 默认作用域
        // 开始解析
        Parser(FileUnit &unit, StringPool &pool, const std::string &szPath);

        // 解析包名
        void GetPackage();
        // 解析导入项
        void GetImport();
        void GetImportItem();

        //------------------声明-------------------

        // 解析声明
        AstDeclRef Declare(ScopeEnum &defaultScope);

        // 解析声明前置标志
        void Front3PScope(ScopeEnum &defaultScope, ScopeEnum &currentScope); // 解析3P作用域
        void FrontFlag(AstFlag &flag);
        // 解析声明后置标志
        void PostFlag(AstFlag &flag);
        // 解析类型
        void Type(AstType &type);

        // 解析函数
        void Function(FunctionDecl &fundecl);
        // 解析参数列表
        void ParamList(std::vector<ParamItem> &list);

        // 解析枚举
        AstDeclRef Enum(ScopeEnum scope);
        // 解析结构体
        AstDeclRef Struct(ScopeEnum scope);
        // 解析委托
        AstDeclRef Entrust(ScopeEnum scope);
        // 解析接口
        AstDeclRef Interface(ScopeEnum scope);
        // 解析类
        AstDeclRef Class(ScopeEnum scope);

        //------------------表达式-------------------
        // 解析表达式
        inline AstExpRef Expression() { return BinaryExpression(-20); }
        // 基础表达式
        AstExpRef BasicExpression();
        // 函数调用表达式
        AstExpRef CallFunExp(StringRef id, TkPos start);
        // 数组访问表达式
        AstExpRef ArrayIndexExp(StringRef id, TkPos start);
        // 一元表达式
        AstExpRef UnaryExpression();
        // 二元表达式
        AstExpRef BinaryExpression(int opPriority);
        // 三元表达式
        AstExpRef TernaryExpression(AstExpRef &cond);

        //------------------语句-------------------
        // 解析块语句
        void BlockStatement(BlockStm &stm);
        // 解析变量定义语句
        bool VarStatement(BlockStm &stm);
        // if*系列语句
        AstStmRef IfStatement();
        AstStmRef ElsifStatement();
        AstStmRef ElseStatement();
        // switch*系列语句
        AstStmRef SwitchStatement();
        AstStmRef CaseStatement();
        AstStmRef DefaultStatement();
        // for*系列语句
        AstStmRef ForStatement();
        AstStmRef WhileStatement();
        AstStmRef DoWhileStatement();
        // try*系列语句
        AstStmRef TryStatement();
        AstStmRef CatchStatement();
        AstStmRef FinallyStatement();
    };
}

#endif //!__PARSER__H__