#include <gtest/gtest.h>
#include "FileScanner.h"
#include "SearchEngine.h"

TEST(FileScannerTest, BasicScan) {
    TinyFileSearch::FileScanner scanner;
    scanner.setRootPath(".");
    EXPECT_TRUE(scanner.scan());
}

TEST(SearchEngineTest, BasicSearch) {
    TinyFileSearch::SearchEngine engine;
    auto files = std::make_shared<std::vector<TinyFileSearch::FileInfo>>();
    engine.setFileData(files);
    auto results = engine.search("test");
    EXPECT_TRUE(results.empty());
}
