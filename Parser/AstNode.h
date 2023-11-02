#ifndef __ASTNODE__H__
#define __ASTNODE__H__
#include <Lexer/Token.h>
#include <Utils/StringPool.h>
namespace air
{
    // 依赖文件声明
    struct ImportItems
    {
        std::set<StringRef> mNames;            // 引用的别名
        std::set<StringRef> mFiles;            // 引用的文件
        std::map<StringRef, StringRef> mItems; // map<别名,文件>
    };

    // 语法抽象树基类
    struct IAstNode
    {
        TkPos mStartPos; // 在源码中的开始的Token的位置
        TkPos mEndPos;   // 在源码中的结束的Token的位置
        bool mIsErr;     // 是否是错误节点

        IAstNode() : mStartPos(), mEndPos(), mIsErr(false) {}

        // 获取错误标记
        inline bool IsError() const { return mIsErr; }
        // 标记位错误节点
        inline void SetError() { mIsErr = true; }
    };

    // 作用域
    enum class ScopeEnum : uint32_t
    {
        Unknown,
        Public,
        Protected,
        Private,
    };

    // 符号类别
    enum class SymbolKind : uint32_t
    {
        Unknown,

        File,    // 文件别名
        Var,     // 变量
        Func,    // 函数
        Enum,    // 枚举类型
        Struct,  // 结构体
        Entrust, // 委托
        Class,   // 结构体
    };
    struct FileUnit;
    // 符号
    struct AstSymbol
    {
        StringRef mName;  // 符号名称
        SymbolKind mKind; // 符号类别

        union
        {
            uint32_t mAttributes; // 属性
            struct
            {
                ScopeEnum mScope : 2;   // 作用域
                uint32_t mIsStatic : 1; // 是否为静态的
            };
        };

        union
        {
            uintptr_t mDef;  // 定义指针
            IAstNode *mNode; // 语句节点项
            FileUnit *mFile; // 文件依赖项
        };

        uint32_t mSize;  // 类型大小
        uint32_t mAlign; // 对齐大小

        AstSymbol(StringRef name, SymbolKind kind, uintptr_t def)
            : mName(name), mKind(kind), mDef(def),
              mAttributes(0), mSize(0), mAlign(0) {}
    };

    // 符号表
    using AstSymbolTable = std::map<StringRef, AstSymbol>;

    // 表达式的值
    struct ExpValue
    {
        StringRef mType;
        union
        {
            int64_t ival;
            uint64_t uval;
            flt64_t fval;
        };
        ExpValue()
            : mType(nullptr), uval(0) {}
    };
    // 表达式类别
    enum class ExpKind : uint32_t
    {
        Unknown, // 未知表达式

        Unary,   // 一元表达式
        Binary,  // 二元表达式
        Ternary, // 三元表达式

        Identity, // 标识符表达式

        Int,     // 有符号常量表达式
        Uint,    // 无符号常量表达式
        Float,   // 浮点数常量表达式
        Char,    // 字符 常量表达式
        CString, // 字符串常量表达式
        Null,    // null表达式
        Bool,    // true/false 常量表达式

        New,     // new表达式
        FunCall, // 函数调用表达式
        Cast,    // 类型转换表达式
        This,    // this 表达式
        Super,   // super 表达式

        Dot,   // 成员访问表达式
        Paren, // 括号表达式
        Array, // 数组索引表达式
        Rang,  // 范围表达式
        Block, // 块表达式

        Lambda, // 匿名表达式

    };

    // 表达式基类
    struct IAstExp : public IAstNode
    {
        ExpValue mVaule; // 常量值
        bool mIsCstVal;
        IAstExp(ExpKind kind = ExpKind::Unknown)
            : IAstNode(), mExpKind(kind), mIsCstVal(false) {}

        inline ExpKind GetKind() { return mExpKind; }
        inline bool IsConstValue() const { return mIsCstVal == true; }

    protected:
        ExpKind mExpKind; // 表达式类别
    };

    // 表达式智能指针
    using AstExpRef = std::shared_ptr<IAstExp>;

    // 生成表达式智能指针
    template <typename Exp, typename... Arg>
    AstExpRef GenExp(Exp *&instance, Arg... arg)
    {
        instance = new Exp(arg...);
        return AstExpRef(instance);
    }

    // 声明标志
    union AstFlag
    {
        AstFlag() : mFlag(0) {}

        uint32_t mFlag;
        struct
        {
            // 通用标记
            ScopeEnum mScope : 2; // 3P 作用域
            uint32_t mStatic : 1; // 静态 static 标识
            // uint32_t mConst : 1;  // 常量标识

            // 函数标记
            uint32_t mInline : 1;   // 内联标识
            uint32_t mVirtual : 1;  // 虚函数
            uint32_t mPureVir : 1;  // 纯虚函数
            uint32_t mOverride : 1; // 函数重写
            uint32_t mFinal : 1;    // 子类不再重写虚函数函数
            uint32_t mNoRet : 1;    // 无返回值
        };
    };

    struct SysType;
    // 类型
    struct AstType
    {
        AstType() : mFlag(0), mSymbol(nullptr), mSysType(nullptr) {}
        union
        {
            uint32_t mFlag;
            struct
            {
                uint32_t mReference : 1; // 是否是引用
                uint32_t mConst : 1;     // 变量常量化
                uint32_t mReadOnly : 1;  // 变量内部只读
                uint32_t mArrFlg : 1;    // 是否是数组
                uint32_t mArrBlk : 1;    // 动态块数组
                uint32_t mArrDynCol : 2; // 动态数组维度
            };
        };

        std::vector<StringRef> mName; // 类型字符串：用于后期查找类型 file.class
                                      // 分析查找的类型
        AstSymbol *mSymbol;           // 用户自定义类型
        SysType *mSysType;            // 基本系统类型

        uint32_t mSize;  // 类型大小
        uint32_t mAlign; // 对齐大小
    };

    // 声明类别
    enum class DeclKind : uint32_t
    {
        Unknown,   // 未知声明
        Var,       // 变量声明
        Func,      // 函数声明
        Enum,      // 枚举声明
        Struct,    // 结构体声明
        Union,     // 联合体声明
        Class,     // 类声明
        Interface, // 接口声明
        Entrust,   // 函数指针声明

    };

    // 声明基类
    struct IAstDecl : public IAstNode
    {
        ScopeEnum mScope; // 3P作用域
        bool mIsDecl;     // 只是声明
        IAstDecl(DeclKind kind = DeclKind::Unknown)
            : IAstNode(), mDeclKind(kind), mScope(ScopeEnum::Unknown), mIsDecl(false) {}

        inline DeclKind GetKind() { return mDeclKind; }

    protected:
        DeclKind mDeclKind; // 表达式类别
    };

    // 声明智能指针
    using AstDeclRef = std::shared_ptr<IAstDecl>;
    // 生成声明智能指针
    template <typename Decl, typename... Arg>
    AstDeclRef GenDecl(Decl *&instance, Arg... arg)
    {
        instance = new Decl(arg...);
        return AstDeclRef(instance);
    }

    // 语句类别
    enum class StmKind : uint32_t
    {
        Unknown, // 未知语句
        Block,   // 块语句

        If,    // if语句
        Else,  // else语句
        Elsif, // elsif语句

        Switch,  // switch语句
        Case,    // case语句
        Default, // default语句

        For,     // for语句
        Foreach, // foreach语句
        While,   // while语句
        DoWhile, // do{}while语句

        Try,     // try语句
        Trow,    // trow语句
        Catch,   // catch语句
        Finally, // finally语句

        Label,    // 标签语句
        Goto,     // goto语句
        Break,    // break语句
        Continue, // continue语句
        Return,   // return语句

        Exp, // 表达式语句
        Var, // 变量声明语句

    };

    // 语句基类
    struct IAstStm : public IAstNode
    {
        IAstStm(StmKind kind = StmKind::Unknown)
            : IAstNode(), mStmKind(kind) {}

        inline StmKind GetKind() { return mStmKind; }

    protected:
        StmKind mStmKind; // 表达式类别
    };
    // 语句智能指针
    using AstStmRef = std::shared_ptr<IAstStm>;
    // 生成声明智能指针
    template <typename Stm, typename... Arg>
    AstStmRef GenStm(Stm *&instance, Arg... arg)
    {
        instance = new Stm(arg...);
        return AstStmRef(instance);
    }

    // 块语句类型
    enum class BlockKind : uint32_t
    {
        Unknown, // 未知

        Function, // 函数块
        Normal,   // 普通块

        If,
        Elsif,
        Else,

        Switch,
        Case,
        Default,

        For,
        Foreach,
        While,
        DoWhile,

        Try,
        Catch,
        Finally,

    };
    // 块语句
    struct BlockStm : public IAstStm
    {
        BlockStm(BlockKind kind, StmKind stm = StmKind::Block) : IAstStm(stm), mBlockKind(kind) {}

        std::list<AstStmRef> mStatement; // 块内部语句

        BlockKind mBlockKind; // 块类型

        AstSymbolTable mSymTable; // 符号表
    };
    //----------------------声明-------------
    // 变量声明
    struct VariableDecl : public IAstDecl
    {
        AstFlag mFlag; // 变量权限标记
        AstType mType; // 变量类型信息

        StringRef mName; // 变量名称

        uint32_t mBitFiled; // 位域长度

        AstExpRef mInitExp; // 初始值表达式

        std::vector<AstExpRef> mArrCol; // 静态数组维度值表达式

        std::vector<AstDeclRef> mVarItems; // 同行逗号定义的变量

        VariableDecl() : IAstDecl(DeclKind::Var), mBitFiled(0)
        {
        }
    };

    // 函数参数
    struct ParamItem
    {
        AstType mType;   // 参数类型
        StringRef mName; // 参数名称
    };
    // 函数声明
    struct FunctionDecl : public IAstDecl
    {
        AstFlag mFlag;                  // 函数权限标记
        AstType mRetType;               // 函数返回值类型
        StringRef mName;                // 函数名称
        std::vector<ParamItem> mParams; // 参数声明列表
        BlockStm mFuncBody;             // 函数体

        FunctionDecl() : IAstDecl(DeclKind::Func), mFuncBody(BlockKind::Function)
        {
        }
    };
    // 枚举项
    struct EnumItem
    {
        StringRef mName;   // 枚举名称
        AstExpRef mValExp; // 枚举值
    };
    // 枚举声明
    struct EnumDecl : public IAstDecl
    {
        StringRef mName;                       // 枚举类型名称
        StringRef mBaseType;                   // 基础类型
        std::vector<EnumItem> mEnums;          // 枚举项声明列表
        std::map<StringRef, uint64_t> mSymVal; // 枚举符号和枚举值的映射
        EnumDecl() : IAstDecl(DeclKind::Enum)
        {
        }
    };

    // 成员变量声明项
    enum class MemberItemKind
    {
        Unknown,
        Var,    // 变量成员
        Union,  // 联合成员 union{};
        Struct, // 结构成员 struct{};
        Func,   // 函数成员
    };

    struct IMemberItem
    {
        MemberItemKind mKind; // 成员类别
        IMemberItem(MemberItemKind kind) : mKind(kind) {}
    };
    // 成员智能指针
    using AstMemberRef = std::shared_ptr<IMemberItem>;
    // 生成成员智能指针
    template <typename Member, typename... Arg>
    AstMemberRef GenMember(Member *&instance, Arg... arg)
    {
        instance = new Member(arg...);
        return AstMemberRef(instance);
    }

    // 变量成员
    struct MemberVar : public IMemberItem
    {
        uint32_t mOffset;   // 偏移
        VariableDecl mDecl; // 成员变量声明
        MemberVar() : IMemberItem(MemberItemKind::Var), mOffset(0) {}
    };
    // Union成员
    struct MemberUnion : public IMemberItem
    {
        std::vector<AstMemberRef> mItems; // 内部成员项
        MemberUnion() : IMemberItem(MemberItemKind::Union) {}
    };
    // Struct成员
    struct MemberStruct : public IMemberItem
    {
        std::vector<AstMemberRef> mItems; // 内部成员项
        MemberStruct() : IMemberItem(MemberItemKind::Struct) {}
    };
    // 变量函数
    struct MemberFunc : public IMemberItem
    {
        FunctionDecl mDecl; // 成员变量声明
        MemberFunc() : IMemberItem(MemberItemKind::Func) {}
    };

    // 结构体声明
    struct StructDecl : public IAstDecl
    {
        StringRef mName;                // 结构体名称
        std::vector<StringRef> mParent; // 父类结构体

        std::vector<AstMemberRef> mMembers; // 成员声明
        ScopeEnum mDefaultScope;            // 默认成员作用域

        AstSymbol *mParentSym;    // 父类定义
        AstSymbolTable mSymTable; // 符号表

        StructDecl() : IAstDecl(DeclKind::Struct),
                       mDefaultScope(ScopeEnum::Public), mParentSym(nullptr) {}
    };
    // 委托声明
    struct EntrustDecl : public IAstDecl
    {
        StringRef mName;                // 委托名称
        AstType mReturn;                // 函数返回值
        bool mIsObj;                    // 对象函数委托
        std::vector<ParamItem> mParams; // 参数声明列表
        EntrustDecl() : IAstDecl(DeclKind::Entrust)
        {
        }
    };

    struct InterfaceItem
    {
        std::vector<StringRef> mName; // 接口名称
    };
    // 接口声明
    struct InterfaceDecl : public IAstDecl
    {
        StringRef mName;                        // 接口名称
        std::vector<FunctionDecl> mMembers;     // 函数声明
        std::vector<InterfaceItem> mInterfaces; // 接口列表

        AstSymbolTable mSymTable; // 符号表

        InterfaceDecl() : IAstDecl(DeclKind::Interface) {}
    };
    // 类声明
    struct ClassDecl : public IAstDecl
    {
        StringRef mName;                        // 类名称
        std::vector<StringRef> mParent;         // 父类
        std::vector<InterfaceItem> mInterfaces; // 接口列表
        std::vector<AstMemberRef> mMembers;     // 成员声明
        ScopeEnum mDefaultScope;                // 默认成员作用域

        AstSymbolTable mSymTable; // 符号表

        ClassDecl() : IAstDecl(DeclKind::Class), mDefaultScope(ScopeEnum::Public) {}
    };
    /* // 实例化声明
     struct UsingDecl : public IAstDecl
     {
         UsingDecl() : IAstDecl(DeclKind::Unknown)
         {
         }
     };*/
    //----------------------表达式-------------

    // 标识符表达式
    struct IdentExp : public IAstExp
    {
        IdentExp(StringRef id)
            : mID(id),
              IAstExp(ExpKind::Identity) {}

        StringRef mID; // 标识符
    };

    // 一元表达式
    struct UnaryExp : public IAstExp
    {
        UnaryExp(bool prev, OpEnum op, AstExpRef &exp)
            : IAstExp(ExpKind::Unary),
              mOp(op), mExp(exp), mPrev(prev) {}
        bool mPrev; // true：前置操作符
        OpEnum mOp; // 操作符
        AstExpRef mExp;
    };
    // 二元表达式
    struct BinaryExp : public IAstExp
    {
        BinaryExp(OpEnum op, AstExpRef &left, AstExpRef right)
            : IAstExp(ExpKind::Binary),
              mOp(op), mLeft(left), mRight(right) {}
        OpEnum mOp;
        AstExpRef mLeft;
        AstExpRef mRight;
    };
    // 三元表达式
    struct TernaryExp : public IAstExp
    {
        TernaryExp()
            : IAstExp(ExpKind::Ternary) {}
        AstExpRef mCond;    // 条件表达式
        AstExpRef mSelect;  // 选择表达式1
        AstExpRef mSelect2; // 选择表达式2
    };
    // 有符号常量表达式
    struct IntExp : public IAstExp
    {
        StringRef mType; // 类型
        int64_t mValue;  // 常量值

        IntExp(StringRef type, int64_t val) : IAstExp(ExpKind::Int), mType(type), mValue(val) {}
    };
    // 无符号常量表达式
    struct UintExp : public IAstExp
    {
        StringRef mType; // 类型
        uint64_t mValue; // 常量值
        UintExp(StringRef type, uint64_t val) : IAstExp(ExpKind::Uint), mType(type), mValue(val) {}
    };
    // 浮点数常量表达式
    struct FloatExp : public IAstExp
    {
        StringRef mType; // 类型
        flt64_t mValue;  // 常量值
        FloatExp() : IAstExp(ExpKind::Float) {}
    };
    // 字符串常量表达式
    struct StringExp : public IAstExp
    {
        StringRef mVal;
        StringExp() : IAstExp(ExpKind::CString) {}
    };
    // 字符常量表达式
    struct CharExp : public IAstExp
    {
        uint32_t mVal;
        CharExp() : IAstExp(ExpKind::Char) {}
    };
    // 空指针常量表达式
    struct NullExp : public IAstExp
    {
        NullExp() : IAstExp(ExpKind::Null) {}
    };
    // 布尔常量表达式
    struct BoolExp : public IAstExp
    {
        uint32_t mVal; // 布尔值
        BoolExp(bool istrue) : IAstExp(ExpKind::Bool), mVal(istrue) {}
    };

    // 括号表达式
    struct ParenExp : public IAstExp
    {
        AstExpRef mExp;
        ParenExp() : IAstExp(ExpKind::Paren) {}
    };

    // 函数调用表达式
    struct FunCallExp : public IAstExp
    {
        StringRef mName;              // 函数名称
        std::vector<AstExpRef> mArgs; // 参数列表
        FunCallExp(StringRef name) : IAstExp(ExpKind::FunCall), mName(name) {}
    };
    // 对象分配表达式
    struct NewExp : public IAstExp
    {
        NewExp() : IAstExp(ExpKind::New) {}
    };

    // 类型转换表达式
    struct CastExp : public IAstExp
    {
        AstType mType;  // 转换类型
        AstExpRef mExp; // 转换的表达式
        CastExp() : IAstExp(ExpKind::Cast) {}
    };
    // 本实例指针表达式
    struct ThisSuperExp : public IAstExp
    {

        ThisSuperExp(ExpKind kind) : IAstExp(kind) {}

        inline bool IsThis() { return GetKind() == ExpKind::This; }
        inline bool IsSuper() { return GetKind() == ExpKind::Super; }
    };
    // 成员访问表达式
    struct DotCallExp : public IAstExp
    {
        std::vector<AstExpRef> mItems; // 成员集
        bool mFunCall;                 // 存在函数调用
        DotCallExp() : IAstExp(ExpKind::Dot), mFunCall(false) {}
    };
    // 数组访问表达式
    struct ArrayExp : public IAstExp
    {
        StringRef mName;                // 数组名称
        std::vector<AstExpRef> mIndexs; // 索引列表
        bool mBlock;                    // true:块数组 false:交错数组
        ArrayExp(StringRef name) : IAstExp(ExpKind::Array), mName(name), mBlock(false) {}
    };
    // 访问表达式:[a,b）
    struct RangExp : public IAstExp
    {
        bool mLeft;       // false：左开 ( true：左闭 [
        bool mRight;      // false：右开 ) true：右闭 ]
        AstExpRef mSmall; // 小的约束：a
        AstExpRef mLarge; // 大的约束：b
        RangExp() : IAstExp(ExpKind::Rang) {}
    };
    // 块表达式项
    struct BlockExpItem
    {
        AstExpRef mID;  // 成员或者索引
        AstExpRef mVal; // 值表达式
    };
    // 块表达式
    struct BlockExp : public IAstExp
    {
        bool mIndex; // 是否是成员索引形式:{exp:exp,Exp:{}}
        std::vector<BlockExpItem> mItems;
        BlockExp() : IAstExp(ExpKind::Block), mIndex(false) {}
    };

    // 匿名表达式
    struct LambdaExp : public IAstExp
    {
        LambdaExp() : IAstExp(ExpKind::Lambda) {}
    };

    //----------------------语句-------------
    // 变量定义语句
    struct VarStm : public IAstStm
    {
        VarStm() : IAstStm(StmKind::Var) {}
        AstDeclRef mVarItem; // 变量定义
    };
    // 表达式语句
    struct ExpStm : public IAstStm
    {
        ExpStm(AstExpRef exp) : IAstStm(StmKind::Exp), mExp(exp) {}

        AstExpRef mExp; // 表达式

        std::vector<AstExpRef> mExpItems; // 逗号表达式
    };
    // if语句
    struct IfStm : public IAstStm
    {
        IfStm() : IAstStm(StmKind::If), mBlock(BlockKind::If) {}

        BlockStm mBlock; // 块语句

        AstExpRef mCondExp; // 条件表达式

        std::vector<AstStmRef> mElsifs;
        AstStmRef mElse;
    };
    // else语句
    struct ElsifStm : public IAstStm
    {
        ElsifStm() : IAstStm(StmKind::Elsif), mBlock(BlockKind::Elsif) {}
        AstExpRef mCondExp; // 条件表达式
        BlockStm mBlock;    // 块语句
    };
    // elsif语句
    struct ElseStm : public IAstStm
    {
        ElseStm() : IAstStm(StmKind::Else), mBlock(BlockKind::Else) {}

        BlockStm mBlock; // 块语句
    };
    // switch语句
    struct SwitchStm : public BlockStm
    {
        SwitchStm() : BlockStm(BlockKind::Switch) {}
        AstExpRef mCondExp;            // 条件表达式
        std::vector<AstStmRef> mCases; // 选择语句集
        AstStmRef mDefault;            // 默认处理语句
    };
    // case语句
    struct CaseStm : public IAstStm
    {
        CaseStm() : IAstStm(StmKind::Case), mBlock(BlockKind::Case) {}
        AstExpRef mCondExp; // 条件表达式
        BlockStm mBlock;    // 块语句
    };
    // default语句
    struct DefaultStm : public IAstStm
    {
        DefaultStm() : IAstStm(StmKind::Default), mBlock(BlockKind::Default) {}

        BlockStm mBlock; // 块语句
    };
    // for语句
    struct ForStm : public IAstStm
    {
        ForStm() : IAstStm(StmKind::For), mBlock(BlockKind::For) {}
        AstStmRef mInitExp;             // 初始条件表达式
        AstExpRef mCondExp;             // 结束条件表达式
        std::vector<AstExpRef> mValExp; // 值更新表达式
        BlockStm mBlock;                // 块语句
    };
    // foreach语句
    struct ForeachStm : public IAstStm
    {
        ForeachStm() : IAstStm(StmKind::Foreach), mBlock(BlockKind::Foreach) {}
        BlockStm mBlock; // 块语句
    };
    // while语句
    struct WhileStm : public IAstStm
    {
        WhileStm() : IAstStm(StmKind::While), mBlock(BlockKind::While) {}
        BlockStm mBlock;    // 块语句
        AstExpRef mCondExp; // 结束条件表达式
    };
    // do{}while语句
    struct DoWhileStm : public IAstStm
    {
        DoWhileStm() : IAstStm(StmKind::DoWhile), mBlock(BlockKind::DoWhile) {}
        BlockStm mBlock;    // 块语句
        AstExpRef mCondExp; // 结束条件表达式
    };
    // try语句
    struct TryStm : public IAstStm
    {
        TryStm() : IAstStm(StmKind::Try), mBlock(BlockKind::Try) {}

        BlockStm mBlock; // 块语句

        std::vector<AstStmRef> mCatchs;
        AstStmRef mFinally;
    };
    // trow语句
    struct TrowStm : public IAstStm
    {
        TrowStm() : IAstStm(StmKind::Trow) {}

        AstExpRef mExp; // 抛出异常表达式
    };
    // catch语句
    struct CatchStm : public IAstStm
    {
        CatchStm() : IAstStm(StmKind::Catch), mBlock(BlockKind::Catch) {}

        VariableDecl mVar;
        BlockStm mBlock; // 块语句
    };
    // finally语句
    struct FinallyStm : public IAstStm
    {
        FinallyStm() : IAstStm(StmKind::Finally), mBlock(BlockKind::Finally) {}

        BlockStm mBlock; // 块语句
    };
    // 标签语句
    struct LabelStm : public IAstStm
    {
        LabelStm(StringRef label) : IAstStm(StmKind::Label), mLabel(label) {}
        StringRef mLabel; // 标签名称
    };
    // goto语句
    struct GotoStm : public IAstStm
    {
        GotoStm() : IAstStm(StmKind::Goto), mLabel() {}
        StringRef mLabel; // 标签名称
    };
    // break语句
    struct BreakStm : public IAstStm
    {
        BreakStm() : IAstStm(StmKind::Break) {}
    };
    // continue语句
    struct ContinueStm : public IAstStm
    {
        ContinueStm() : IAstStm(StmKind::Continue) {}
    };
    // return语句
    struct ReturnStm : public IAstStm
    {
        ReturnStm() : IAstStm(StmKind::Return) {}

        AstExpRef mVal; // 返回值
    };

}

#endif //!__ASTNODE__H__