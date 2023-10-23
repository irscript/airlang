#ifndef __SEMANTIC__H__
#define __SEMANTIC__H__
#include <Utils/FileUnit.h>
#include <Lexer/Scanner.h>
#include <Utils/ErrorExp.h>
#include <Utils/Project.h>
namespace air
{
    struct Semantic
    {
        FileCenter &mCenter; // 文件中心
        FileUnit &mUnit;     // 文件单元
        StringPool &mPool;   // 字符串池
    };
}
#endif //!__SEMANTIC__H__
