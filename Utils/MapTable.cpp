#include "MapTable.h"

namespace air
{
    std::map<std::string, KeyEnum> MapTable::KeyMap;     // 关键字
    std::map<std::string, MacroEnum> MapTable::MacroMap; // 宏关键字
    std::map<OpEnum, int32_t> MapTable::OpPriorityMap;   // 操作符优先级

    void MapTable::InitKeyMap()
    {
        // KeyMap.insert({"",KeyEnum::});
        KeyMap.insert({"package", KeyEnum::Package});
        KeyMap.insert({"import", KeyEnum::Import});
        KeyMap.insert({"using", KeyEnum::Using});

        KeyMap.insert({"void", KeyEnum::Void});
        KeyMap.insert({"any", KeyEnum::Any});
        KeyMap.insert({"bool", KeyEnum::Bool});

        KeyMap.insert({"int8", KeyEnum::Int8});
        KeyMap.insert({"int16", KeyEnum::Int16});
        KeyMap.insert({"int32", KeyEnum::Int32});
        KeyMap.insert({"int64", KeyEnum::Int64});

        KeyMap.insert({"uint8", KeyEnum::UInt8});
        KeyMap.insert({"uint16", KeyEnum::UInt16});
        KeyMap.insert({"uint32", KeyEnum::UInt32});
        KeyMap.insert({"uint64", KeyEnum::UInt64});

        KeyMap.insert({"flt32", KeyEnum::Flt32});
        KeyMap.insert({"flt64", KeyEnum::Flt64});

        KeyMap.insert({"cstring", KeyEnum::CString});
        KeyMap.insert({"string", KeyEnum::String});
        KeyMap.insert({"char", KeyEnum::Char});

        KeyMap.insert({"uintptr", KeyEnum::Uintptr});
        KeyMap.insert({"sint", KeyEnum::SInt});
        KeyMap.insert({"uint", KeyEnum::UInt});

        KeyMap.insert({"nullptr", KeyEnum::Nullptr});
        KeyMap.insert({"false", KeyEnum::False});
        KeyMap.insert({"true", KeyEnum::True});

        KeyMap.insert({"in", KeyEnum::In});

        KeyMap.insert({"break", KeyEnum::Break});
        KeyMap.insert({"continue", KeyEnum::Continue});
        KeyMap.insert({"goto", KeyEnum::Goto});
        KeyMap.insert({"return", KeyEnum::Return});

        KeyMap.insert({"do", KeyEnum::Do});
        KeyMap.insert({"while", KeyEnum::While});
        KeyMap.insert({"for", KeyEnum::For});

        KeyMap.insert({"if", KeyEnum::If});
        KeyMap.insert({"elsif", KeyEnum::Elsif});
        KeyMap.insert({"else", KeyEnum::Else});

        KeyMap.insert({"switch", KeyEnum::Switch});
        KeyMap.insert({"case", KeyEnum::Case});
        KeyMap.insert({"default", KeyEnum::Default});

        KeyMap.insert({"private", KeyEnum::Private});
        KeyMap.insert({"protected", KeyEnum::Protected});
        KeyMap.insert({"public", KeyEnum::Public});

        KeyMap.insert({"static", KeyEnum::Static});
        KeyMap.insert({"const", KeyEnum::Const});
        KeyMap.insert({"readonly", KeyEnum::Readonly});
        KeyMap.insert({"inline", KeyEnum::Inline});
        KeyMap.insert({"virtual", KeyEnum::Virtual});
        KeyMap.insert({"override", KeyEnum::Override});

        KeyMap.insert({"enum", KeyEnum::Enum});
        KeyMap.insert({"union", KeyEnum::Union});
        KeyMap.insert({"struct", KeyEnum::Struct});
        KeyMap.insert({"class", KeyEnum::Class});
        KeyMap.insert({"interface", KeyEnum::Interface});
        KeyMap.insert({"entrust", KeyEnum::Entrust});

        KeyMap.insert({"this", KeyEnum::This});
        KeyMap.insert({"super", KeyEnum::Super});
        KeyMap.insert({"friend", KeyEnum::Friend});

        KeyMap.insert({"new", KeyEnum::New});
        KeyMap.insert({"sizeof", KeyEnum::Sizeof});
        KeyMap.insert({"isa", KeyEnum::IsA});
        KeyMap.insert({"cast", KeyEnum::Cast});

        KeyMap.insert({"try", KeyEnum::Try});
        KeyMap.insert({"catch", KeyEnum::Catch});
        KeyMap.insert({"finally", KeyEnum::Finally});
        KeyMap.insert({"throw", KeyEnum::Throw});

        KeyMap.insert({"async", KeyEnum::Async});
        KeyMap.insert({"await", KeyEnum::Await});
        KeyMap.insert({"synchronized", KeyEnum::Synchronized});
        KeyMap.insert({"volatile", KeyEnum::Volatile});
    }

    void MapTable::InitMacroMap()
    {
        // MacroMap.insert({"",MacroEnum::});
        MacroMap.insert({"@file", MacroEnum::File});
        MacroMap.insert({"@line", MacroEnum::Line});
        MacroMap.insert({"@func", MacroEnum::Func});
        MacroMap.insert({"@debug", MacroEnum::Debug});

        MacroMap.insert({"@libimport", MacroEnum::LibImport});
        MacroMap.insert({"@dllimport", MacroEnum::DllImport});
    }
    void MapTable::InitOpPriorityMap()
    {
        // * / %
        OpPriorityMap.insert({OpEnum::Multiply, 0});
        OpPriorityMap.insert({OpEnum::Divide, 0});
        OpPriorityMap.insert({OpEnum::Modulus, 0});
        // + -
        OpPriorityMap.insert({OpEnum::Plus, -1});
        OpPriorityMap.insert({OpEnum::Minus, -1});

        // << >> >>>
        OpPriorityMap.insert({OpEnum::LeftShift, -2});
        OpPriorityMap.insert({OpEnum::RightShift, -2});
        OpPriorityMap.insert({OpEnum::LogicalRightShift, -2});

        // < <= > >=
        OpPriorityMap.insert({OpEnum::LT, -3});
        OpPriorityMap.insert({OpEnum::LE, -3});
        OpPriorityMap.insert({OpEnum::GT, -3});
        OpPriorityMap.insert({OpEnum::GE, -3});

        // == !=
        OpPriorityMap.insert({OpEnum::EQ, -4});
        OpPriorityMap.insert({OpEnum::NE, -4});

        // &
        OpPriorityMap.insert({OpEnum::BitAnd, -5});

        // ^
        OpPriorityMap.insert({OpEnum::BitXOr, -6});

        // |
        OpPriorityMap.insert({OpEnum::BitOr, -7});

        // &&
        OpPriorityMap.insert({OpEnum::And, -8});

        // ||
        OpPriorityMap.insert({OpEnum::Or, -9});

        // =
        OpPriorityMap.insert({OpEnum::Assign, -10});                  // =
        OpPriorityMap.insert({OpEnum::PlusAssign, -10});              //+=
        OpPriorityMap.insert({OpEnum::MinusAssign, -10});             //-=
        OpPriorityMap.insert({OpEnum::MulAssign, -10});               //*=
        OpPriorityMap.insert({OpEnum::DivAssign, -10});               // /=
        OpPriorityMap.insert({OpEnum::ModAssign, -10});               // %=
        OpPriorityMap.insert({OpEnum::LeftShiftAssign, -10});         // >>=
        OpPriorityMap.insert({OpEnum::RightShiftAssign, -10});        // <<=
        OpPriorityMap.insert({OpEnum::LogicalRightShiftAssign, -10}); //<<<=
        OpPriorityMap.insert({OpEnum::BitAndAssign, -10});            // &=
        OpPriorityMap.insert({OpEnum::BitOrAssign, -10});             // |=
        OpPriorityMap.insert({OpEnum::BitXOrAssign, -10});            // ^=
        OpPriorityMap.insert({OpEnum::BitNotAssign, -10});            // ~=
    }
}
