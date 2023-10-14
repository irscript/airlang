#ifndef __LOGER__H__
#define __LOGER__H__
#include <Utils/Pch.h>
namespace air
{
    void Error(const char *fmt, ...);
    void Waring(const char *fmt, ...);
    void Info(const char *fmt, ...);
    void Print(const char *fmt, ...);
}
#endif //!__LOGER__H__