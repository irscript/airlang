#include <Utils/StringPool.h>
#include <Utils/MapTable.h>
#include <Lexer/Scanner.h>
#include <Utils/Project.h>
#include <Utils/FileHelp.h>
#include <Utils/Loger.h>
// #include <Parser/AstNode.h>
using namespace std;
using namespace air;

int main(int argc, char **argv)
{
    system("chcp 65001");
    system("cls");

    MapTable::Init();

    /*
    Scanner scan;
    air::ReadFile("E:\\Project\\Compiler\\airlang\\lexer.test.air", scan.GetStream());

    while (1)
    {
        auto tok = scan.GetNext();
        if (tok.kind == TkKind::Eof)
            break;
        printf("%s\n", tok.ToString().c_str());
    }*/

    auto &projMgr = ProjectCenter::Instance();
    projMgr.Init();
    // 加载项目配置
    if (projMgr.LoadFile("G:\\Compiler\\airlang\\project.aircfg") == false)
    {
        Error("解析项目配置文件失败！\n");
        return 0;
    }
    // 打印项目配置
    // projMgr.DumpCfg();

    Print("\n开始编译程序");
    Print("\n-----------------------------------------------------------\n\n");
    // 编译项目
    projMgr.Start();

    Print("\n结束编译程序");
    Print("\n-----------------------------------------------------------\n\n");

    return 0;
}
