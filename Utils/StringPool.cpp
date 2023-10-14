#include <Utils/StringPool.h>

namespace air
{
    const StringRef StringPool::RefString(const std::string &szRefs)
    {
        auto res = mPools.insert(Item(szRefs, 1));

        if (res.second == false)
            res.first->second += 1;

        return (StringRef)&res.first->first;
    }

    void StringPool::UnRefString(const StringRef szRefs)
    {
        if (szRefs == nullptr)
            return;
        auto it = mPools.find(*szRefs);
        if (it != mPools.end())
        {
            --it->second;
            if (it->second == 0)
                mPools.erase(it);
        }
    }
}