#include <Utils/FileHelp.h>
#include <direct.h>
#include <windows.h>
#include "FileHelp.h"
namespace air
{
    void CheckPath(std::string &szPath)
    {
        std::string szTmp;
        auto size = szPath.size();
        for (size_t i = 0; i < size; ++i)
        {
            if (szPath[i] == '\\')
                szTmp.push_back('/');
            else
                szTmp.push_back(szPath[i]);
        }
        // 确保以 / 结尾
        if (szTmp[size - 1] != '/')
            szTmp.push_back('/');
        szPath = szTmp;
    }

    std::string GetWorkDir()
    {
        std::string dir = getcwd(nullptr, 0);
        CheckPath(dir);
        return dir;
    }
    std::string GetAppDir()
    {
        char buffer[2048] = {0};
        int rst = GetModuleFileNameA(NULL, buffer, sizeof(buffer));
        std::string path = buffer;
        CheckPath(path);
        path.pop_back();
        auto pos = path.find_last_of('/');
        auto dir = path.substr(0, pos + 1);
        return dir;
    }
    bool PathExist(const std::string &szFile)
    {
        /* if (access(szFile.c_str(), 0) == 0)
             return true;
         return false;
         */
        DWORD dwAttrib = GetFileAttributesA(szFile.c_str());

        return dwAttrib != INVALID_FILE_ATTRIBUTES ;
                //&&!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
    }
    bool ReadFile(const std::string &szFile, std::string &szTxt)
    {
        FILE *fp = nullptr;
        // 打开文件
        if (fopen_s(&fp, szFile.c_str(), "rb") != 0)
            return false;

        // 获取文件大小，分配缓存
        fseek(fp, 0, SEEK_END);
        size_t size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        szTxt.resize(size);
        // 读取文件
        fread_s(szTxt.data(), szTxt.size(),
                sizeof(char), szTxt.size(), fp);
        // 关闭文件
        fclose(fp);
        return true;
    }
    bool WriteFile(const std::string &szFile, void *data, size_t len)
    {
        FILE *fp = nullptr;
        // 打开文件
        if (fopen_s(&fp, szFile.c_str(), "wb") != 0)
            return false;
        // 写文件
        fwrite(data, sizeof(char), len, fp);
        // 关闭文件
        fclose(fp);
        return true;
    }
}
