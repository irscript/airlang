#ifndef __ERROREXP__H__
#define __ERROREXP__H__

#include <Utils/Pch.h>

namespace air
{
    // 错误信息
    struct ErrorExpception
    {
        auto GetWhat() { return mMsg.c_str(); }
        auto &GetString() { return mMsg; }

        ErrorExpception(const char *msg) : mMsg(msg) {}
        ErrorExpception(const std::string &msg) : mMsg(msg) {}

    protected:
        std::string mMsg; // 错误信息
    };
}

#endif //!__ERROREXP__H__