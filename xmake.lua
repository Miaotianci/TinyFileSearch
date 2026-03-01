target("tinyfilesearch")
    set_kind("binary")
    set_languages("c++17")
    set_warnings("all")

    add_includedirs("include")

    add_files("src/main.cpp", "src/FileScanner.cpp", "src/SearchEngine.cpp", "src/FileIndex.cpp")
