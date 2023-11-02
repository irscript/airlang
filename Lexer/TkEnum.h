#ifndef __TKENUM__H__
#define __TKENUM__H__
#include <Utils/Pch.h>

namespace air
{
    // 单词类别枚举
    enum class TkKind : uint16_t
    {
        Error,          // 错误单词
        Macro,          // 宏关键字
        Notes,          // 单行注释
        MultiNotes,     // 多行注释
        KeyWord,        // 关键字
        Identifier,     // 标识符
        StringLiteral,  // 字符串常量
        CharLiteral,    // 字符常量
        IntegerLiteral, // 整形字符串常量
        FloatLiteral,   // 浮点字符串常量
        Seperator,      // 分隔符
        Operaor,        // 操作符
        Eof,            // 文件结束
        Max
    };

    // 分隔符枚举
    enum class SpEnum : uint16_t
    {
        Unknown,
        OpenBracket,  // [
        CloseBracket, // ]
        OpenParen,    // (
        CloseParen,   // )
        OpenBrace,    // {
        CloseBrace,   // }
        Colon,        // :
        SemiColon,    // ;
        Comma,        // ,
        Ellipsis,     // ...
    };
    // 操作符枚举
    enum class OpEnum : uint16_t
    {
        Unknown,
        Dot,      // .
        Question, // ?

        RightShift,        // <<
        LeftShift,         // >>
        LogicalRightShift, // <<<

        BitAnd, // &
        BitXOr, // ^
        BitOr,  // |
        BitNot, // ~

        Not, // !
        And, // &&
        Or,  // ||

        Plus,     // +
        Minus,    // -
        Multiply, // *
        Divide,   // /
        Modulus,  // %

        Plus2,  // ++
        Minus2, // --

        Assign,      // =
        PlusAssign,  // +=
        MinusAssign, // -=
        MulAssign,   // *=
        DivAssign,   // /=
        ModAssign,   // %=

        RightShiftAssign,        // <<=
        LeftShiftAssign,         // >>=
        LogicalRightShiftAssign, // <<<=

        BitAndAssign, // &=
        BitXOrAssign, // ^=
        BitOrAssign,  // |=
        BitNotAssign, // ~=

        EQ, // ==
        NE, // !=
        GT, // >
        GE, // >=
        LT, // <
        LE, // <=

    };
    // 关键字枚举
    enum class KeyEnum : uint16_t
    {
        Unknown,
        // 顶层语法
        Package,
        Import,
        Using,

        // 基本类型
        Void,
        Any,
        Bool,
        Int8,
        Int16,
        Int32,
        Int64,
        UInt8,
        UInt16,
        UInt32,
        UInt64,
        SInt,    // 根据目标架构，可变有符号
        UInt,    // 根据目标架构，可变无符号
        Flt32,   // 32位浮点
        Flt64,   // 64位浮点
        Uintptr, // 地址
        String,  // 字符串
        CString, // 常量字符串
        Char,    // utf8字符

        //  基本值
        Nullptr, // 空地址
        False,
        True,

        In, // 范围内: x in [1,6)

        // 基本跳转
        Break,
        Continue,
        Goto,
        Return,

        // 循环
        Do,
        While,
        For,
        // 条件
        If,
        Elsif,
        Else,
        Switch,
        Case,
        Default,

        // 修饰
        Private,   // 私有的
        Protected, // 受保护的
        Public,    // 公开的
        Static,    // 静态的
        Const,     // 常量（不可修改）的
        Readonly,  // 只读的
        Inline,    // 内联的
        Override,  // 重写的
        Virtual,   // 虚函数

        // 扩展类型
        Enum,
        Union,
        Struct,
        Class,
        Interface,
        Entrust,

        This,   // 实例成员访问指针
        Super,  // 父类成员访问指针
        Friend, // 友元声明

        New,    // 分配一个对象
        Sizeof, // 获取类型大小
        IsA,    // 判断是否是该类型的实例
        Cast,   // 类型转换

        // 异常
        Try,
        Catch,
        Finally,
        Throw,

        // 同步异步
        Async,        // 异步调用函数
        Await,        // 等待异步唤醒
        Synchronized, // 同步代码块
        Volatile,
    };
    // 宏关键字枚举
    enum class MacroEnum : uint16_t
    {
        Unknown,
        File,  // @file 获取当前文件名
        Line,  // @line 获取当前代码行号
        Func,  // @func 获取当前函数名称
        Debug, // @debug 标识当前编译模式为debug下才使用的代码块

        LibImport, // @libimport 静态库声明导入
        DllImport, // @dllimport 动态库声明导入

        StdCall,  // 函数调用约定
        Cdecl,    // 函数调用约定
        FastCall, // 函数调用约定
    };
}
#endif //!__TKENUM__H__