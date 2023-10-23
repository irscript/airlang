#ifndef __IRNODE__H__
#define __IRNODE__H__
#include <Utils/Pch.h>
#include <Parser/AstNode.h>
// 中间代码结构定义
namespace air
{
    struct IIRNode;
    using IRNodeRef = std::shared_ptr<IIRNode>;

    enum class IRNodeKind : uint32_t
    {
        Unknown,
        // 基本块节点
        Block,

        // 基本块子类别
        FunctionBlock, // 函数块
        PublicBlock,   // 普通块
        LoopBlock,     // 循环块

        // IR节点
        Node,
        // 一元节点
        assign, // 赋值=
        neg,    // 符号取反 -
        inv,    // 按位取反 ~
        abs,    // 去绝对值

        // 二元节点
        add, // +
        sub, // -
        mul, // *
        div, // /
        mod, // %

        bitsand, // &
        bitsor,  // |
        bitsxor, // ^

        shl,  // >> 左移
        lshl, // << 逻辑右移
        ashr, // <<< 算术右移

        // 函数调用节点

        Max
    };

    // IR 节点访问者
    struct IIRVisit;
    // IR节点基类
    struct IIRNode
    {
        IRNodeKind mKind; // 类别

        IIRNode(IRNodeKind kind) : mKind(kind) {}

        std::vector<IRNodeRef> mUsed; // 被哪些节点使用

        // 是否是基本块
        bool IsBlock() const { return mKind > IRNodeKind::Block && mKind < IRNodeKind::Node; }

        // 节点是否未被使用
        bool IsNotUsed() const { return mKind != IRNodeKind::FunctionBlock && mUsed.size() == 0; }

        // 节点访问
        virtual void Visit(IIRVisit &vist) = 0;
    };
    // 符号
    struct IRSymbol
    {
        uint32_t mIsArg : 1; // 是否是参数
        AstType mType;       // 类型
        StringRef mName;     // 名称
    };
    // 值类型
    enum class IRTypeKind
    {
        Unknown,

        Bool,

        Int8,
        Int16,
        Int32,
        Int64,

        Uint8,
        Uint16,
        Uint32,
        Uint64,

        Flt32,
        Flt64,

        String, // 字符串

        Object,  // 对象
        Pointer, // 指针
    };
    // 值
    struct IRValue
    {

        uint32_t mIsConst : 1;  // 为常量？
        uint32_t mIsSymbol : 1; // 为符号？
        uint32_t mDef : 16;     // 第几个定义

        IRTypeKind mType; // 值类型

        StringRef mSymbol; // 符号名称

        union
        {
            int32_t i32;
            int64_t i64;
            uint32_t u32;
            uint64_t u64;
            flt32_t f32;
            flt64_t f64;
        };
        StringRef mString; // 字符串常量
    };
    // 基本块
    struct IRBlock;
    // 一元节点
    struct UnaryIRNode;
    // 二元节点
    struct BinaryIRNode;
    // IR 节点访问者
    struct IIRVisit
    {
        virtual void Visit(IIRNode &node) = 0;
        virtual void Visit(IRBlock &block) = 0;
        virtual void Visit(UnaryIRNode &unary) = 0;
        virtual void Visit(BinaryIRNode &binary) = 0;
    };
    // 基本块
    struct IRBlock : IIRNode
    {
        IRBlock(IRNodeKind kind) : IIRNode(kind) {}
        std::list<IRNodeRef> mNodes; // 块内节点

        // 是不是循环块
        bool IsLoop() const { return mKind == IRNodeKind::LoopBlock; }

        void Visit(IIRVisit &vist) override { vist.Visit(*this); }
    };
    // 一元节点
    struct UnaryIRNode : IIRNode
    {
        UnaryIRNode(IRNodeKind kind) : IIRNode(kind) {}

        IRValue mSrc; // 源操作数
        IRValue mDes; // 目的操作数

        void Visit(IIRVisit &vist) override { vist.Visit(*this); }
    };
    // 二元节点
    struct BinaryIRNode : IIRNode
    {
        BinaryIRNode(IRNodeKind kind) : IIRNode(kind) {}
        IRValue mSrc;  // 源操作数
        IRValue mSrc2; // 源操作数
        IRValue mDes;  // 目的操作数

        void Visit(IIRVisit &vist) override { vist.Visit(*this); }
    };
}
#endif //!__IRNODE__H__