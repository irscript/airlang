#include "Semantic.h"

namespace air
{
    void Semanticer::Start()
    {
        // 标注已经开始进行符号表建立
        mUnit.mIsSymbol = 1;
        // 第一步：文件依赖
        Import();

        // 第二步：各类声明
        for (auto &item : mUnit.mAstTree)
        {
            switch (item->GetKind())
            {
            case DeclKind::Var:
                Var(*(VariableDecl *)item.get());
                break;
            case DeclKind::Func:
                Fun(*(FunctionDecl *)item.get());
                break;
            case DeclKind::Enum:
                Enum(*(EnumDecl *)item.get());
                break;
            case DeclKind::Entrust:
                Entrust(*(EntrustDecl *)item.get());
                break;
            case DeclKind::Struct:
                Struct(*(StructDecl *)item.get());
                break;
            case DeclKind::Class:
                Class(*(ClassDecl *)item.get());
                break;

            default:
                break;
            }
        }
    }

    void Semanticer::Import()
    {
        for (auto &item : mUnit.mImports.mItems)
        {
            auto &rely = mCenter.GetFileUint(item.second);
            auto res = mUnit.mSymTable.insert({item.first, AstSymbol(item.first, SymbolKind::File, (uintptr_t)&rely)});
            if (res.second == false)
            {
                Error("symbol： %s\n", item.first->c_str());
                throw ErrorExpception("符号重定义!");
            }
            // 如果依赖文件未建立符号表
            if (rely.mIsSymbol == false)
            {
                Semanticer anly(rely, mCenter, mPool, mSysType, mAddressSize);
            }
        }
    }

    void Semanticer::CheckType(AstType &type)
    {
        // 基本类型？
        if (type.mName.size() == 1)
        {
            auto find = mSysType.find(type.mName.front());
            if (find != mSysType.end())
            {
                type.mSysType = &find->second;
                if (type.mArrFlg)
                {
                    type.mSize = mAddressSize;
                    type.mAlign = mAddressSize;
                }
                // 基本类型
                else
                {
                    type.mSize = type.mSysType->mSize;
                    type.mAlign = type.mSysType->mAlign;
                }
                return;
            }
        }
        // 用户自定义类型
        AstSymbol *symbol = nullptr;
        auto finds = mUnit.FindSymbol(type.mName[0], symbol);
        if (finds == false)
        {
            Error("file: %s\n", mUnit.mFile->c_str());
            Error("symbol： %s\n", type.mName[0]->c_str());
            throw ErrorExpception("未知符号!");
        }
        switch (symbol->mKind)
        {
        case SymbolKind::File:
        {
            if (type.mName.size() == 1)
            {
                Error("file: %s\n", mUnit.mFile->c_str());
                Error("symbol： %s\n", type.mName[0]->c_str());
                throw ErrorExpception("符号不是类型!");
            }
            if (symbol->mFile->FindSymbol(type.mName[1], symbol) == false)
            {
                Error("file: %s\n", mUnit.mFile->c_str());
                Error("symbol： %s.%s\n", type.mName[0]->c_str(), type.mName[1]->c_str());
                throw ErrorExpception("未知符号!");
            }
            if (symbol->mKind < SymbolKind::Enum || symbol->mKind > SymbolKind::Class)
            {
                Error("file: %s\n", mUnit.mFile->c_str());
                Error("symbol： %s.%s\n", type.mName[0]->c_str(), type.mName[1]->c_str());
                throw ErrorExpception("符号不是类型!");
            }
            type.mSymbol = symbol;
        }
        break;
        case SymbolKind::Enum:
        {
            type.mSymbol = symbol;
        }
        break;
        case SymbolKind::Entrust:
        {
            type.mSymbol = symbol;
        }
        break;
        case SymbolKind::Struct:
        {
            type.mSymbol = symbol;
        }
        break;
        case SymbolKind::Class:
        {
            type.mSymbol = symbol;
            // 检查类型是否完整
            if (symbol == nullptr && type.mReference == 0)
            {
                Error("file: %s\n", mUnit.mFile->c_str());
                Error("symbol： %s.%s\n", type.mName[0]->c_str(), type.mName[1]->c_str());
                throw ErrorExpception("不允许使用不完整类型!");
                break;
            }
        }
        break;

        default:
            Error("file: %s\n", mUnit.mFile->c_str());
            Error("symbol： %s.%s\n", type.mName[0]->c_str(), type.mName[1]->c_str());
            throw ErrorExpception("未知符号!");
            break;
        }

        // 计算类型大小
        if (type.mArrFlg)
        {
            type.mSize = mAddressSize;
            type.mAlign = mAddressSize;
        }
        else
        {
            if (type.mSymbol->mKind != SymbolKind::Class)
            {
                type.mSize = type.mSymbol->mSize;
                type.mAlign = type.mSymbol->mAlign;
            }
            else
            {
                type.mSize = mAddressSize;
                type.mAlign = mAddressSize;
            }
        }
    }

    bool Semanticer::GetExpValue(AstExpRef &exp)
    {
        return false;
    }

    void Semanticer::Var(VariableDecl &var)
    {
        auto res = mUnit.mSymTable.insert({var.mName, AstSymbol(var.mName, SymbolKind::Var, (uintptr_t)&var)});
        if (res.second == false)
        {
            Error("file: %s\n", mUnit.mFile->c_str());
            Error("line: %d\n", var.mStartPos.mLine);
            Error("symbol： %s\n", var.mName->c_str());
            throw ErrorExpception("符号重定义!");
        }
        // 检查类型
        CheckType(var.mType);
        if (var.mType.mSysType != nullptr &&
            var.mType.mSysType->mName->compare("void") == 0)
        {
            Error("file: %s\n", mUnit.mFile->c_str());
            Error("line: %d\n", var.mStartPos.mLine);
            Error("var %s\n", var.mName->c_str());
            throw ErrorExpception("变量不能使用 'void' 类型!");
        }
        // 初始值表达式后期检查
        if (var.mType.mSysType != nullptr && var.mType.mArrFlg == 0)
        {
            GetExpValue(var.mInitExp);
        }

        // 同行逗号定义的变量
        for (auto &item : var.mVarItems)
            Var(*(VariableDecl *)item.get());
    }
    void Semanticer::Fun(FunctionDecl &fun)
    {
        auto res = mUnit.mSymTable.insert({fun.mName, AstSymbol(fun.mName, SymbolKind::Func, (uintptr_t)&fun)});
        if (res.second == false)
        {
            Error("file: %s\n", mUnit.mFile->c_str());
            Error("line: %d\n", fun.mStartPos.mLine);
            Error("symbol： %s\n", fun.mName->c_str());
            throw ErrorExpception("符号重定义!");
        }
        // 返回值检查
        CheckType(fun.mRetType);
        if (fun.mRetType.mSysType != nullptr &&
            fun.mRetType.mSysType->mName->compare("void") == 0)
            fun.mFlag.mNoRet = 1;

        // 参数类型检查
        for (auto &parm : fun.mParams)
        {
            CheckType(parm.mType);
            if (parm.mType.mSysType != nullptr &&
                parm.mType.mSysType->mName->compare("void") == 0)
            {
                Error("file: %s\n", mUnit.mFile->c_str());
                Error("line: %d\n", fun.mStartPos.mLine);
                Error("func %s\n", fun.mName->c_str());
                throw ErrorExpception("参数不能使用 'void' 类型!");
            }
        }
    }
    void Semanticer::Enum(EnumDecl &enm)
    {
        auto res = mUnit.mSymTable.insert({enm.mName, AstSymbol(enm.mName, SymbolKind::Enum, (uintptr_t)&enm)});
        if (res.second == false)
        {
            Error("file: %s\n", mUnit.mFile->c_str());
            Error("line: %d\n", enm.mStartPos.mLine);
            Error("symbol： %s\n", enm.mName->c_str());
            throw ErrorExpception("符号重定义!");
        }
        // 分析枚举类型的大小
        auto find = mSysType.find(enm.mBaseType);
        if (find == mSysType.end())
        {
            Error("file: %s\n", mUnit.mFile->c_str());
            Error("line: %d\n", enm.mStartPos.mLine);
            Error("symbol： %s\n", enm.mName->c_str());
            Error("type: %d\n", enm.mBaseType->c_str());
            throw ErrorExpception("未知基本类型!");
        }
        auto &sym = res.first->second;
        sym.mSize = find->second.mSize;
        sym.mAlign = find->second.mAlign;
        // 枚举项的值，后续分析
    }
    void Semanticer::Entrust(EntrustDecl &ent)
    {
        auto res = mUnit.mSymTable.insert({ent.mName, AstSymbol(ent.mName, SymbolKind::Enum, (uintptr_t)&ent)});
        if (res.second == false)
        {
            Error("file: %s\n", mUnit.mFile->c_str());
            Error("line: %d\n", ent.mStartPos.mLine);
            Error("symbol： %s\n", ent.mName->c_str());
            throw ErrorExpception("符号重定义!");
        }
        auto &sym = res.first->second;
        // 区分是静态函数还是成员函数
        sym.mSize = ent.mIsObj == true ? mAddressSize * 2 : mAddressSize;
        sym.mAlign = mAddressSize;

        // 返回值检查
        CheckType(ent.mReturn);
        // 参数类型检查
        for (auto &parm : ent.mParams)
        {
            CheckType(parm.mType);
            if (parm.mType.mSysType != nullptr &&
                parm.mType.mSysType->mName->compare("void") == 0)
            {
                Error("file: %s\n", mUnit.mFile->c_str());
                Error("line: %d\n", ent.mStartPos.mLine);
                Error("entrust %s\n", ent.mName->c_str());
                throw ErrorExpception("参数不能使用 'void' 类型!");
            }
        }
    }
    void Semanticer::Struct(StructDecl &stc)
    {
        auto res = mUnit.mSymTable.insert({stc.mName, AstSymbol(stc.mName, SymbolKind::Struct, (uintptr_t)&stc)});
        if (res.second == false)
        {
            Error("file: %s\n", mUnit.mFile->c_str());
            Error("line: %d\n", stc.mStartPos.mLine);
            Error("symbol： %s\n", stc.mName->c_str());
            throw ErrorExpception("符号重定义!");
        }
        auto &sym = res.first->second;
        // 查找父类
        if (stc.mParent.empty() == false)
        {
            AstSymbol *symbol = nullptr;
            auto find = mUnit.FindSymbol(stc.mParent[0], symbol);
            if (find == false)
            {
                Error("file: %s\n", mUnit.mFile->c_str());
                Error("symbol： %s\n", stc.mParent[0]->c_str());
                throw ErrorExpception("未知符号!");
            }
            switch (symbol->mKind)
            {
            case SymbolKind::File:
            {
            }
            break;
            case SymbolKind::Struct:
            {
                stc.mParentSym = symbol;
            }
            break;
            default:
            {
                Error("file: %s\n", mUnit.mFile->c_str());
                Error("symbol： %s\n", stc.mParent[0]->c_str());
                throw ErrorExpception("符号非结构体类型!");
            }
            break;
            }
        }

        uint32_t size = stc.mParentSym == nullptr ? 0 : stc.mParentSym->mSize;
        uint32_t align = stc.mParentSym == nullptr ? 4 : stc.mParentSym->mAlign;

        // 建立符号表并计算成员偏移，类型大小
        for (auto &item : stc.mMembers)
        {
            switch (item->mKind)
            {
            case MemberItemKind::Var:
            {
                MemberVar &var = *(MemberVar *)item.get();
                auto res = stc.mSymTable.insert({stc.mName, AstSymbol(stc.mName, SymbolKind::Var, (uintptr_t)&var.mDecl)});
                if (res.second == false)
                {
                    Error("file: %s\n", mUnit.mFile->c_str());
                    Error("line: %d\n", stc.mStartPos.mLine);
                    Error("symbol： %s\n", stc.mName->c_str());
                    throw ErrorExpception("符号重定义!");
                }
                // 更新结构体对齐
                if (align < var.mDecl.mType.mAlign)
                    align = var.mDecl.mType.mAlign;
                // 大小对齐
                auto tmp = var.mDecl.mType.mAlign - 1;
                size = (size + tmp) & (~(tmp));
                // 计算变量大小
                uint32_t varsize = var.mDecl.mType.mSize;
                // 数组？
                size += varsize;
            }
            break;
            case MemberItemKind::Func:
            {
            }
            break;
            case MemberItemKind::Union:
            {
            }
            break;
            default:
                break;
            }
        }
    }
    void Semanticer::Class(ClassDecl &cls)
    {
        auto res = mUnit.mSymTable.insert({cls.mName, AstSymbol(cls.mName, SymbolKind::Class, (uintptr_t)&cls)});
        if (res.second == false)
        {
            Error("file: %s\n", mUnit.mFile->c_str());
            Error("line: %d\n", cls.mStartPos.mLine);
            Error("symbol： %s\n", cls.mName->c_str());
            throw ErrorExpception("符号重定义!");
        }
        auto &sym = res.first->second;
    }
}
