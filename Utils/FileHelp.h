#ifndef __FILEHELP__H__
#define __FILEHELP__H__
#include <Utils/Pch.h>

namespace air
{
    // 校验路径:确保使用 / 分割且结尾
    void CheckPath(std::string &szPath);
    // 获取工作路径
    std::string GetWorkDir();
    // 获取app路径
    std::string GetAppDir();
    // 文件/目录是否存在
    bool PathExist(const std::string &szFile);
    // 读取文件
    bool ReadFile(const std::string &szFile, std::string &szTxt);
    // 写文件
    bool WriteFile(const std::string &szFile, void *data, size_t len);


}
#endif //!__FILEHELP__H__