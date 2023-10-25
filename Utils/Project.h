#ifndef __PROJECT__H__
#define __PROJECT__H__

#include <Utils/Pch.h>
#include <Lexer/Scanner.h>
#include <Utils/FileUnit.h>
namespace air
{
    // 系统类型
    struct SysType
    {
        StringRef mName;  // 类型名称
        uint32_t mSize;   // 类型大小：按照字节算
        uint32_t mAllign; // 类型对齐大小：按照字节算
    };

    // 项目编译组织单元
    struct Project
    {
        std::string mName;            // 项目名称
        std::string mArch;            // 目标架构
        std::string mPlat;            // 目标平台
        std::string mMode;            // 编译模式
        std::string mType;            // 输出的类型
        std::string mBuildDir;        // 构建输出的目录
        std::set<std::string> mDirs;  // 搜索的目录
        std::set<std::string> mFiles; // 编译的文件

        std::map<StringRef, SysType> mSysType; // 系统类型

        uint32_t mAddressSize; // 地址大小

        FileCenter mFileUnits; // 编译文件中心

        StringPool mStringPool; // 字符串池

        std::string mFilePath; // 解析时用于构建文件路径

        // 解析项目
        void ParserProj();
        // 获取文件的唯一路径
        void GetFilePath(const std::string &szFile);
        // 解析文件
        void ParserFile(const std::string &szFile);

        // 初始化系统类型系统: 地址大小按照字节算
        void InitSysType(uint32_t addrsize);
    };

    struct ProjectCenter
    {
        std::list<Project> mProjs;
        std::string mWorkDir; // 工作目录
        std::string mAppDir;  // 编译器目录

        // 初始化
        void Init();

        // 生成一个项目节点
        Project &GenProject() { return mProjs.emplace_back(); }

        // 从文件中获取项目配置
        bool LoadFile(const std::string &szPath);
        // 打印项目配置
        void DumpCfg();

        // 开始解析项目
        void Start();

        static ProjectCenter &Instance() { return mInstance; }

    protected:
        bool ParserProject(Scanner &scan);

        static ProjectCenter mInstance;
    };
}

#endif //!__PROJECT__H__