#ifndef __STRINGPOOL__H__
#define __STRINGPOOL__H__
#include <Utils/Pch.h>

namespace air
{

    using StringRef = const std::string *;
    // 字符串引用池
    class StringPool
    {
    private:
        // map<字符串 ,引用次数>
        using Pool = std::map<std::string, uint32_t>;
        using Item = std::pair<std::string, uint32_t>;

        Pool mPools;

    public:
        StringPool() {}
        ~StringPool() { mPools.clear(); }

        // 添加一个字符串引用,不存在就插入，存在就引用次数+1
        const StringRef RefString(const std::string &szRefs);
        // 释放一个字符串引用，存在就引用次数-1,引用次数为0时释放字符串,返回释放后引用次数
        void UnRefString(const StringRef szRefs);
    };

}
#endif //!__STRINGPOOL__H__
