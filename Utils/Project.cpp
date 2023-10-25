
#include <Utils/Project.h>
#include <Lexer/Scanner.h>
#include "Project.h"
#include <Utils/FileHelp.h>
#include <Parser/Parser.h>
#include <Parser/AstDump.h>
namespace air
{
    ProjectCenter ProjectCenter::mInstance;

    void ProjectCenter::Init()
    {
        mWorkDir = GetWorkDir();
        mAppDir = GetAppDir();
    }
    bool air::ProjectCenter::LoadFile(const std::string &szPath)
    {
        Scanner scan;
        auto res = ReadFile(szPath, scan.GetStream());
        if (res == false)
        {
            Error("读取项目配置文件[%s]失败!\n", szPath.c_str());
            return false;
        }
        while (true)
        {
            auto tok = scan.GetNext();
            if (tok.kind == TkKind::Identifier)
            {
                if (tok.text != "project")
                {
                    Error("项目配置文件格式不正确! 应输入 'project' : %s\n", tok.ToString().c_str());
                    return false;
                }
                // 开始解析项目配置
                if (ParserProject(scan) == false)
                    return false;
            }
            else if (tok.kind == TkKind::Eof)
                break; // 解析完成
        }

        return true;
    }
    void ProjectCenter::DumpCfg()
    {
        for (auto it = mProjs.begin(); it != mProjs.end(); ++it)
        {
            Info("project(%s){\n", it->mName.c_str());
            Info("\tarch: %s\n", it->mArch.c_str());
            Info("\tplat: %s\n", it->mPlat.c_str());
            Info("\tmode: %s\n", it->mMode.c_str());
            Info("\ttype: %s\n", it->mType.c_str());
            Info("\tbuild: %s\n", it->mBuildDir.c_str());

            Info("\tdirs{\n");
            for (auto dit = it->mDirs.begin();
                 dit != it->mDirs.end(); ++dit)
            {
                Info("\t\t%s\n", dit->c_str());
            }
            Info("\t}\n");

            Info("\tfiles{\n");
            for (auto fit = it->mFiles.begin();
                 fit != it->mFiles.end(); ++fit)
            {
                Info("\t\t%s\n", fit->c_str());
            }
            Info("\t}\n");

            Info("}\n");
        }
    }
    void ProjectCenter::Start()
    {
        for (auto project : mProjs)
        {
            try
            {
                Info("\n编译项目: %s \n+++++++++++++++++++++++++++++++++\n", project.mName.c_str());
                // 语法解析
                project.ParserProj();
                // 语义分析
                // project.Semantic();
            }
            catch (ErrorExpception exp)
            {
                Error("解析项目[ %s ]时发生异常:%s\n", project.mName.c_str(), exp.GetWhat());
                break;
            }
            catch (...)
            {
                Error("解析项目时发生未知异常！");
                break;
            }
        }
    }
    bool ProjectCenter::ParserProject(Scanner &scan)
    {
        auto &proj = GenProject();

        // 解析项目名称
        auto tok = scan.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenParen)
        {
            Error("项目配置文件格式不正确! 应输入 '(' : %s\n", tok.ToString().c_str());
            return false;
        }
        tok = scan.GetNext();
        if (tok.kind != TkKind::StringLiteral)
        {
            Error("项目配置文件格式不正确! 应输入 项目名称字符串,如'lang' : %s\n", tok.ToString().c_str());
            return false;
        }
        proj.mName = tok.text;
        tok = scan.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::CloseParen)
        {
            Error("项目配置文件格式不正确! 应输入 '(' : %s\n", tok.ToString().c_str());
            return false;
        }
        // 开始解析项目描述体
        tok = scan.GetNext();
        if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenBrace)
        {
            Error("项目配置文件格式不正确! 应输入 '{' : %s\n", tok.ToString().c_str());
            return false;
        }
        while (1)
        {
            tok = scan.GetNext();
            if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseBrace)
            {
                Print("解析项目[ %s ]配置成功！\n", proj.mName.c_str());
                break;
            }
            else if (tok.kind == TkKind::Identifier)
            {
                if (tok.text == "arch")
                {
                    tok = scan.GetNext();
                    if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::Colon)
                    {
                        Error("项目配置文件格式不正确! 应输入 ':' : %s\n", tok.ToString().c_str());
                        return false;
                    }
                    tok = scan.GetNext();
                    if (tok.kind != TkKind::Identifier)
                    {
                        Error("项目配置文件格式不正确! 应输入 架构标志符 : %s\n", tok.ToString().c_str());
                        return false;
                    }
                    proj.mArch = tok.text;
                    continue;
                }
                if (tok.text == "plat")
                {
                    tok = scan.GetNext();
                    if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::Colon)
                    {
                        Error("项目配置文件格式不正确! 应输入 ':' : %s\n", tok.ToString().c_str());
                        return false;
                    }
                    tok = scan.GetNext();
                    if (tok.kind != TkKind::Identifier)
                    {
                        Error("项目配置文件格式不正确! 应输入 平台标志符 : %s\n", tok.ToString().c_str());
                        return false;
                    }
                    proj.mPlat = tok.text;
                    continue;
                }
                if (tok.text == "mode")
                {
                    tok = scan.GetNext();
                    if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::Colon)
                    {
                        Error("项目配置文件格式不正确! 应输入 ':' : %s\n", tok.ToString().c_str());
                        return false;
                    }
                    tok = scan.GetNext();
                    if (tok.kind != TkKind::Identifier)
                    {
                        Error("项目配置文件格式不正确! 应输入 编译模式[ debug | release ] : %s\n", tok.ToString().c_str());
                        return false;
                    }
                    proj.mMode = tok.text;
                    continue;
                }
                if (tok.text == "type")
                {
                    tok = scan.GetNext();
                    if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::Colon)
                    {
                        Error("项目配置文件格式不正确! 应输入 ':' : %s\n", tok.ToString().c_str());
                        return false;
                    }
                    tok = scan.GetNext();
                    if (tok.kind != TkKind::Identifier)
                    {
                        Error("项目配置文件格式不正确! 应输入 输出类型标识符 : %s\n", tok.ToString().c_str());
                        return false;
                    }
                    proj.mType = tok.text;
                    continue;
                }
                if (tok.text == "build")
                {
                    tok = scan.GetNext();
                    if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::Colon)
                    {
                        Error("项目配置文件格式不正确! 应输入 ':' : %s\n", tok.ToString().c_str());
                        return false;
                    }
                    tok = scan.GetNext();
                    if (tok.kind != TkKind::StringLiteral)
                    {
                        Error("项目配置文件格式不正确! 应输入 构建目录 : %s\n", tok.ToString().c_str());
                        return false;
                    }
                    proj.mBuildDir = tok.text;
                    CheckPath(proj.mBuildDir);
                    if (PathExist(proj.mBuildDir) == false)
                        Waring("目录不存在：%s\n", proj.mBuildDir.c_str());
                    continue;
                }
                if (tok.text == "dirs")
                {
                    tok = scan.GetNext();
                    if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenBrace)
                    {
                        Error("项目配置文件格式不正确! 应输入 '{' : %s\n", tok.ToString().c_str());
                        return false;
                    }
                    while (true)
                    {
                        tok = scan.GetNext();
                        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseBrace)
                            break;
                        else if (tok.kind == TkKind::StringLiteral)
                        {
                            CheckPath(tok.text);
                            if (PathExist(tok.text) == false)
                                Waring("目录不存在：%s\n", tok.text.c_str());
                            else
                                proj.mDirs.insert(tok.text);
                            continue;
                        }
                        Error("项目配置文件格式不正确! 应输入 '}' : %s\n", tok.ToString().c_str());
                        return false;
                    }

                    continue;
                }
                if (tok.text == "files")
                {
                    tok = scan.GetNext();
                    if (tok.kind != TkKind::Seperator || tok.code.sp != SpEnum::OpenBrace)
                    {
                        Error("项目配置文件格式不正确! 应输入 '{' : %s\n", tok.ToString().c_str());
                        return false;
                    }
                    while (true)
                    {
                        tok = scan.GetNext();
                        if (tok.kind == TkKind::Seperator && tok.code.sp == SpEnum::CloseBrace)
                            break;
                        else if (tok.kind == TkKind::StringLiteral)
                        {
                            CheckPath(tok.text);
                            tok.text.pop_back();
                            proj.mFiles.insert(tok.text);
                            continue;
                        }
                        Error("项目配置文件格式不正确! 应输入 '}' : %s\n", tok.ToString().c_str());
                        return false;
                    }

                    continue;
                }
            }
            Error("项目配置文件格式不正确! 应输入 '}' : %s\n", tok.ToString().c_str());
            return false;
        }
        proj.mDirs.insert(mAppDir);
        proj.mDirs.insert(mWorkDir);
        return true;
    }

    void Project::ParserProj()
    {
        // 初始化类型系统

        InitSysType(8);

        std::string szPath;
        float icnt = 0;
        uint32_t errcnt = 0; // 错误计数
        uint32_t total = mFiles.size();
        for (auto file : mFiles)
        {
            try
            {
                Info("[ %3u %% ]: ", uint32_t((icnt / mFiles.size()) * 100));
                ParserFile(file);
                ++icnt;
            }
            catch (ErrorExpception exp)
            {
                Error("    异常: %s\n", exp.GetWhat());
                Error("    文件: %s > %s \n", mName.c_str(), file.c_str());
                ++errcnt;
                continue;
            }
            catch (...)
            {
                Error("解析项目时发生未知异常！");
                break;
            }
        }
        if (errcnt == 0)
        {
            Info("[ 100 %% ]: 编译完成！\n");
            for (auto unit : mFileUnits.mFileUnits)
            {
                AstDumper dump(unit.second);
            }
        }

        else
            Print("编译结束！总数：%4u\t正确: %4u\t错误: %4u\n", total, total - errcnt, errcnt);
    }

    void Project::GetFilePath(const std::string &szFile)
    {
        std::list<std::string> pathlist;
        for (auto item : mDirs)
        {
            mFilePath = item + szFile;
            if (PathExist(mFilePath) == true)
                pathlist.push_front(mFilePath);
        }
        if (pathlist.empty() == true)
            throw ErrorExpception("文件不存在！");
        if (pathlist.size() != 1)
        {
            Print("\n文件路径不确定！疑是路径如下：\n");
            for (auto item : mDirs)
                Waring("\t%s\n", item.c_str());
            throw ErrorExpception("文件路径冲突！");
        }
        // 获得唯一路径
        mFilePath = pathlist.front();
    }

    void Project::ParserFile(const std::string &szFile)
    {
        // 获取文件路径
        GetFilePath(szFile);
        // 获取文件名字符串引用
        auto fileRef = mStringPool.RefString(szFile);
        // 检查是否已经解析过文件
        if (mFileUnits.CheckFileUint(fileRef) == true)
            return;

        // 生成对应的文件单元
        auto &unit = mFileUnits.GetFileUint(fileRef);
        // 开始语法解析
        Parser parser(unit, mStringPool, mFilePath);
    }

    void Project::InitSysType(uint32_t addrsize)
    {
        mAddressSize = addrsize;

        auto name = mStringPool.RefString("void");
        mSysType.insert({name, {name, 0, 0}});

        name = mStringPool.RefString("bool");
        mSysType.insert({name, {name, 1, 1}});

        name = mStringPool.RefString("int8");
        mSysType.insert({name, {name, 1, 1}});
        name = mStringPool.RefString("int16");
        mSysType.insert({name, {name, 2, 2}});
        name = mStringPool.RefString("int32");
        mSysType.insert({name, {name, 4, 4}});
        name = mStringPool.RefString("int64");
        mSysType.insert({name, {name, 8, addrsize}});

        name = mStringPool.RefString("uint8");
        mSysType.insert({name, {name, 1, 1}});
        name = mStringPool.RefString("uint16");
        mSysType.insert({name, {name, 2, 2}});
        name = mStringPool.RefString("uint32");
        mSysType.insert({name, {name, 4, 4}});
        name = mStringPool.RefString("uint64");
        mSysType.insert({name, {name, 8, addrsize}});

        name = mStringPool.RefString("sint");
        mSysType.insert({name, {name, addrsize, addrsize}});
        name = mStringPool.RefString("uint");
        mSysType.insert({name, {name, addrsize, addrsize}});

        name = mStringPool.RefString("flt32");
        mSysType.insert({name, {name, 4, 4}});
        name = mStringPool.RefString("flt64");
        mSysType.insert({name, {name, 8, addrsize}});

        name = mStringPool.RefString("uintptr");
        mSysType.insert({name, {name, addrsize, addrsize}});
        name = mStringPool.RefString("cstring");
        mSysType.insert({name, {name, addrsize, addrsize}});
        name = mStringPool.RefString("string");
        mSysType.insert({name, {name, addrsize, addrsize}});
        name = mStringPool.RefString("char");
        mSysType.insert({name, {name, 4, 4}});
    }
}
