#include <Utils/Loger.h>

namespace air
{
#define _loger(clr)                                                \
    va_list ap;                                                    \
    va_start(ap, fmt);                                             \
    std::vector<char> buffer;                                      \
    buffer.resize(2048);                                           \
    vsnprintf_s(buffer.data(), buffer.size(),                      \
                buffer.size(), fmt, ap);                           \
    va_end(ap);                                                    \
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), clr); \
    printf(buffer.data());

    void Error(const char *fmt, ...)
    {
        _loger(0xC);
    }
    void Waring(const char *fmt, ...)
    {
        _loger(0xE);
    }
    void Info(const char *fmt, ...)
    {
        _loger(0xA);
    }
    void Print(const char *fmt, ...)
    {
        _loger(0xF);
    }
}