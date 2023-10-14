add_rules("mode.debug", "mode.release")

set_languages("c++17")

target("airc")
    set_kind("binary")
    add_includedirs(".")
    add_files("**.cpp")
