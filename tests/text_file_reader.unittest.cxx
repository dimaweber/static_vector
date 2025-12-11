#include "text_file_reader.hxx"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <vector>

// Helper class to create and cleanup temporary test files
class TempTestFile {
    std::filesystem::path path_;

public:
    explicit TempTestFile(const std::string& content) {
        path_ = std::filesystem::temp_directory_path() / ("test_" + std::to_string(rand()) + ".txt");
        std::ofstream ofs(path_);
        ofs << content;
        ofs.close();
    }

    ~TempTestFile() {
        if (std::filesystem::exists(path_)) {
            std::filesystem::remove(path_);
        }
    }

    const std::filesystem::path& path() const { return path_; }
};

TEST(TextFileReaderTest, BasicRead) {
    TempTestFile file("line1\nline2\nline3\n");

    wbr::text_file_reader_t<wbr::TextLineReadOpt::None> reader(file.path());

    EXPECT_TRUE(reader.is_open());

    std::vector<std::string> lines;
    for (const auto& line : reader) {
        lines.push_back(line);
    }

    ASSERT_EQ(lines.size(), 3);
    EXPECT_EQ(lines[0], "line1");
    EXPECT_EQ(lines[1], "line2");
    EXPECT_EQ(lines[2], "line3");
}

TEST(TextFileReaderTest, SkipEmptyLines) {
    TempTestFile file("line1\n\nline2\n\n\nline3\n");

    wbr::text_file_reader_t<wbr::TextLineReadOpt::SkipEmpty> reader(file.path());

    std::vector<std::string> lines;
    for (const auto& line : reader) {
        lines.push_back(line);
    }

    ASSERT_EQ(lines.size(), 3);
    EXPECT_EQ(lines[0], "line1");
    EXPECT_EQ(lines[1], "line2");
    EXPECT_EQ(lines[2], "line3");
}

TEST(TextFileReaderTest, SkipComments) {
    TempTestFile file("line1\n# comment\nline2\n#another comment\nline3\n");

    wbr::text_file_reader_t<wbr::TextLineReadOpt::SkipComment> reader(file.path());

    std::vector<std::string> lines;
    for (const auto& line : reader) {
        lines.push_back(line);
    }

    ASSERT_EQ(lines.size(), 3);
    EXPECT_EQ(lines[0], "line1");
    EXPECT_EQ(lines[1], "line2");
    EXPECT_EQ(lines[2], "line3");
}

TEST(TextFileReaderTest, TrimWhitespace) {
    TempTestFile file("  line1  \n\t line2\t\n   line3   \n");

    wbr::text_file_reader_t<wbr::TextLineReadOpt::TrimWhitespace> reader(file.path());

    std::vector<std::string> lines;
    for (const auto& line : reader) {
        lines.push_back(line);
    }

    ASSERT_EQ(lines.size(), 3);
    EXPECT_EQ(lines[0], "line1");
    EXPECT_EQ(lines[1], "line2");
    EXPECT_EQ(lines[2], "line3");
}

TEST(TextFileReaderTest, CombinedOptions) {
    TempTestFile file(
        "  line1  \n"
        "# comment line\n"
        "\n"
        "   line2\n"
        "  # another comment  \n"
        "\n"
        "line3   \n"
    );

    // Default options: SkipComment | SkipEmpty | TrimWhitespace
    wbr::text_file_reader_t<> reader(file.path());

    std::vector<std::string> lines;
    for (const auto& line : reader) {
        lines.push_back(line);
    }

    ASSERT_EQ(lines.size(), 3);
    EXPECT_EQ(lines[0], "line1");
    EXPECT_EQ(lines[1], "line2");
    EXPECT_EQ(lines[2], "line3");
}

TEST(TextFileReaderTest, CommentAfterWhitespace) {
    TempTestFile file("line1\n   # comment with leading spaces\n\t# tab comment\nline2\n");

    wbr::text_file_reader_t<wbr::TextLineReadOpt::SkipComment> reader(file.path());

    std::vector<std::string> lines;
    for (const auto& line : reader) {
        lines.push_back(line);
    }

    ASSERT_EQ(lines.size(), 2);
    EXPECT_EQ(lines[0], "line1");
    EXPECT_EQ(lines[1], "line2");
}

TEST(TextFileReaderTest, EmptyFile) {
    TempTestFile file("");

    wbr::text_file_reader_t<> reader(file.path());

    std::vector<std::string> lines;
    for (const auto& line : reader) {
        lines.push_back(line);
    }

    EXPECT_EQ(lines.size(), 0);
}

TEST(TextFileReaderTest, OnlyEmptyLines) {
    TempTestFile file("\n\n\n\n");

    wbr::text_file_reader_t<wbr::TextLineReadOpt::SkipEmpty> reader(file.path());

    std::vector<std::string> lines;
    for (const auto& line : reader) {
        lines.push_back(line);
    }

    EXPECT_EQ(lines.size(), 0);
}

TEST(TextFileReaderTest, OnlyComments) {
    TempTestFile file("# comment1\n# comment2\n# comment3\n");

    wbr::text_file_reader_t<wbr::TextLineReadOpt::SkipComment> reader(file.path());

    std::vector<std::string> lines;
    for (const auto& line : reader) {
        lines.push_back(line);
    }

    EXPECT_EQ(lines.size(), 0);
}

TEST(TextFileReaderTest, NoOptionsPreservesEverything) {
    TempTestFile file("  line1  \n\n# comment\n   \nline2\n");

    wbr::text_file_reader_t<wbr::TextLineReadOpt::None> reader(file.path());

    std::vector<std::string> lines;
    for (const auto& line : reader) {
        lines.push_back(line);
    }

    ASSERT_EQ(lines.size(), 5);
    EXPECT_EQ(lines[0], "  line1  ");
    EXPECT_EQ(lines[1], "");
    EXPECT_EQ(lines[2], "# comment");
    EXPECT_EQ(lines[3], "   ");
    EXPECT_EQ(lines[4], "line2");
}

TEST(TextFileReaderTest, FileNotFound) {
    wbr::text_file_reader_t<> reader("/nonexistent/file/path.txt");

    EXPECT_FALSE(reader.is_open());

    std::vector<std::string> lines;
    for (const auto& line : reader) {
        lines.push_back(line);
    }

    EXPECT_EQ(lines.size(), 0);
}

TEST(TextFileReaderTest, ConfigFileExample) {
    TempTestFile file(
        "# Configuration file\n"
        "# Lines starting with # are comments\n"
        "\n"
        "server_host = localhost\n"
        "server_port = 8080\n"
        "\n"
        "# Database settings\n"
        "db_name = mydb\n"
        "  db_user = admin  \n"
        "\n"
    );

    wbr::text_file_reader_t<> reader(file.path());

    std::vector<std::string> lines;
    for (const auto& line : reader) {
        lines.push_back(line);
    }

    ASSERT_EQ(lines.size(), 4);
    EXPECT_EQ(lines[0], "server_host = localhost");
    EXPECT_EQ(lines[1], "server_port = 8080");
    EXPECT_EQ(lines[2], "db_name = mydb");
    EXPECT_EQ(lines[3], "db_user = admin");
}

TEST(TextFileReaderTest, RangeAlgorithms) {
    TempTestFile file("line1\nline2\nline3\n");

    wbr::text_file_reader_t<wbr::TextLineReadOpt::None> reader(file.path());

    // Use with std::ranges algorithms
    auto count = std::ranges::count_if(reader, [](const auto& line) {
        return line.find('2') != std::string::npos;
    });

    EXPECT_EQ(count, 1);
}

TEST(TextFileReaderTest, MoveConstructor) {
    TempTestFile file("line1\nline2\n");

    wbr::text_file_reader_t<wbr::TextLineReadOpt::None> reader1(file.path());
    EXPECT_TRUE(reader1.is_open());

    wbr::text_file_reader_t<wbr::TextLineReadOpt::None> reader2(std::move(reader1));
    EXPECT_TRUE(reader2.is_open());

    std::vector<std::string> lines;
    for (const auto& line : reader2) {
        lines.push_back(line);
    }

    EXPECT_EQ(lines.size(), 2);
}

TEST(TextFileReaderTest, FilterPredicates) {
    EXPECT_TRUE(wbr::text_file_reader_t<>::empty_line_filter(std::string("")));
    EXPECT_FALSE(wbr::text_file_reader_t<>::empty_line_filter(std::string("content")));

    EXPECT_TRUE(wbr::text_file_reader_t<>::comment_line_filter(std::string("# comment")));
    EXPECT_FALSE(wbr::text_file_reader_t<>::comment_line_filter(std::string("not a comment")));

    EXPECT_FALSE(wbr::text_file_reader_t<>::not_empty_line_filter(std::string("")));
    EXPECT_TRUE(wbr::text_file_reader_t<>::not_empty_line_filter(std::string("content")));

    EXPECT_FALSE(wbr::text_file_reader_t<>::not_comment_line_filter(std::string("# comment")));
    EXPECT_TRUE(wbr::text_file_reader_t<>::not_comment_line_filter(std::string("not a comment")));
}

TEST(TextFileReaderTest, TrimTransform) {
    auto trimmed = wbr::text_file_reader_t<>::trim_transform(std::string("  hello  "));
    EXPECT_EQ(trimmed, "hello");

    trimmed = wbr::text_file_reader_t<>::trim_transform(std::string("\t\nworld\n\t"));
    EXPECT_EQ(trimmed, "world");
}

TEST(TextFileReaderTest, LargeFile) {
    // Create a file with many lines
    std::string content;
    for (int i = 0; i < 1000; ++i) {
        content += "line" + std::to_string(i) + "\n";
    }

    TempTestFile file(content);
    wbr::text_file_reader_t<wbr::TextLineReadOpt::None> reader(file.path());

    int count = 0;
    for (const auto& line : reader) {
        ++count;
        (void)line; // Suppress unused variable warning
    }

    EXPECT_EQ(count, 1000);
}

TEST(TextFileReaderTest, HashInMiddleOfLine) {
    TempTestFile file("key=value#not_a_comment\nkey2=value2\n");

    wbr::text_file_reader_t<wbr::TextLineReadOpt::SkipComment> reader(file.path());

    std::vector<std::string> lines;
    for (const auto& line : reader) {
        lines.push_back(line);
    }

    // Lines with # in the middle are NOT skipped (only lines starting with #)
    ASSERT_EQ(lines.size(), 2);
    EXPECT_EQ(lines[0], "key=value#not_a_comment");
    EXPECT_EQ(lines[1], "key2=value2");
}

