#include "static_string.hxx"

using namespace wbr;
using namespace testing;

namespace wbr {
TEST (StaticStringAdapterTest, DefaultConstructor) {
    std::array<char, 10> char_array = {'a', 'b', 'c', '\0'};

    static_string_adapter adapter(char_array.data( ), char_array.size( ));
    EXPECT_EQ(adapter.head_, char_array.data( ));
    EXPECT_EQ(adapter.tail_, adapter.head_);
    EXPECT_EQ(adapter.max_length_, char_array.size( ) - 1);
    EXPECT_EQ(*adapter.tail_, '\0');
    EXPECT_EQ(char_array[0], '\0');

    EXPECT_EQ(adapter.length( ), 0);
}

TEST (StaticStringAdapterTest, ConstructorWithLength) {
    std::array<char, 10> char_array = {'a', 'b', 'c', '\0', 'd'};

    static_string_adapter adapter(char_array.data( ), char_array.size( ), 4);
    EXPECT_EQ(adapter.head_, char_array.data( ));
    EXPECT_EQ(adapter.tail_, adapter.head_ + 4);
    EXPECT_EQ(adapter.max_length_, char_array.size( ) - 1);
    EXPECT_EQ(*adapter.tail_, '\0');
    EXPECT_EQ(char_array[0], 'a');
    EXPECT_EQ(char_array[1], 'b');
    EXPECT_EQ(char_array[2], 'c');
    EXPECT_EQ(char_array[3], '\0');
    EXPECT_EQ(char_array[4], '\0');

    EXPECT_EQ(adapter.length( ), 4);
}

TEST (StaticStringAdapterTest, ArrayConstructor) {
    std::array<char, 10> char_array = {'a', 'b', 'c', '\0'};

    static_string_adapter adapter(char_array);
    EXPECT_EQ(adapter.head_, char_array.data( ));
    EXPECT_EQ(adapter.tail_, adapter.head_);
    EXPECT_EQ(adapter.max_length_, char_array.size( ) - 1);
    EXPECT_EQ(*adapter.tail_, '\0');
    EXPECT_EQ(char_array[0], '\0');

    EXPECT_EQ(adapter.length( ), 0);
}

TEST (StaticStringAdapterTest, CArrayConstructor) {
    char c_array[10] = {'a', 'b', 'c', '\0'};

    static_string_adapter adapter(c_array);
    EXPECT_EQ(adapter.head_, c_array);
    EXPECT_EQ(adapter.tail_, adapter.head_);
    EXPECT_EQ(adapter.max_length_, std::size(c_array) - 1);
    EXPECT_EQ(*adapter.tail_, '\0');
    EXPECT_EQ(c_array[0], '\0');

    EXPECT_EQ(adapter.length( ), 0);
}

TEST (StaticStringAdapterTest, BoundaryCheck) {
    std::array<char, 10> char_array;

    EXPECT_THROW(static_string_adapter<BoundCheckStrategy::Exception>(char_array.data( ), 0), std::length_error);
    EXPECT_THROW(static_string_adapter<BoundCheckStrategy::Exception>(nullptr, 0), std::runtime_error);

    //@todo: add test for Assert
    //@todo: add test for limitToBounds
}

TEST (StaticStringAdapterTest, LengthMethod) {
    char                  empty_buffer[50];
    static_string_adapter empty_str {empty_buffer, std::size(empty_buffer)};
    EXPECT_EQ(empty_str.length( ), 0);

    char                  non_empty_buffer[50] = "Hello Kitty";
    static_string_adapter non_empty_str {non_empty_buffer, 50, 5};
    EXPECT_EQ(non_empty_str.length( ), 5);
}

TEST (StaticStringAdapterTest, SizeMethod) {
    char                  empty_buffer[50];
    static_string_adapter empty_str {empty_buffer, std::size(empty_buffer)};
    EXPECT_EQ(empty_str.size( ), 0);

    char                  non_empty_buffer[50] = "Hello Kitty";
    static_string_adapter non_empty_str {non_empty_buffer, 50, 5};
    EXPECT_EQ(non_empty_str.size( ), 5);
}

TEST (StaticStringAdapterTest, MaxSizeMethod) {
    char                  buffer[50];
    static_string_adapter str(buffer, sizeof(buffer));
    EXPECT_EQ(str.max_size( ), 49);  // max_size is capacity - 1 for null terminator

    // Try with a different size
    char                  smaller_buffer[20];
    static_string_adapter small_str(smaller_buffer, sizeof(smaller_buffer));
    EXPECT_EQ(small_str.max_size( ), 19);
}

TEST (StaticStringAdapterTest, OperatorBrackets) {
    char                  buffer[50] = "Hello";
    static_string_adapter str(buffer, sizeof(buffer), 5);

    EXPECT_EQ(str[0], 'H');
    EXPECT_EQ(str[1], 'e');
    EXPECT_EQ(str[4], 'o');

    EXPECT_NO_THROW(auto c = str[-1]);
    EXPECT_NO_THROW(auto c = str[5]);  // Past the string length, but within capacity
}

TEST (StaticStringAdapterTest, AssignIteratorRange) {
    char                                                    buffer[50];
    static_string_adapter<BoundCheckStrategy::LimitToBound> str(buffer, sizeof(buffer));

    // Test assigning a range of iterators
    std::string source = "Hello";
    str.assign(source.begin( ), source.end( ));
    EXPECT_EQ(str.length( ), 5);
    EXPECT_STREQ(str.c_str( ), "Hello");

    // Test with empty range
    str.assign("", "");
    EXPECT_EQ(str.length( ), 0);

    // Test with large range that should be truncated (with LimitToBound strategy)
    std::string large_source(100, 'A');
    str.assign(large_source.begin( ), large_source.end( ));
    EXPECT_EQ(str.length( ), 49);  // Max size is buffer_size - 1
    EXPECT_STREQ(str.c_str( ), "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
}

TEST (StaticStringAdapterTest, AssignCountValue) {
    char                                                    buffer[50];
    static_string_adapter<BoundCheckStrategy::LimitToBound> str(buffer, sizeof(buffer));

    // Test assigning count and value
    str.assign(5, 'X');
    EXPECT_EQ(str.length( ), 5);
    EXPECT_STREQ(str.c_str( ), "XXXXX");

    // Test with zero count
    str.assign(0, 'Y');
    EXPECT_EQ(str.length( ), 0);

    // Test with large count that should be truncated (with LimitToBound strategy)
    str.assign(100, 'Z');
    EXPECT_EQ(str.length( ), 49);  // Max size is buffer_size - 1
    EXPECT_STREQ(str.c_str( ), "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ");
}

TEST (StaticStringAdapterTest, AssignStringView) {
    using namespace std::literals;
    char                                                    buffer[50];
    static_string_adapter<BoundCheckStrategy::LimitToBound> str(buffer, sizeof(buffer));

    // Test assigning from string_view
    str.assign("World"sv);
    EXPECT_EQ(str.length( ), 5);
    EXPECT_STREQ(str.c_str( ), "World");

    // Test with empty string_view
    str.assign("");
    EXPECT_EQ(str.length( ), 0);

    // Test with large string_view that should be truncated (with LimitToBound strategy)
    std::string large_source(100, 'B');
    str.assign(std::string_view {large_source});
    EXPECT_EQ(str.length( ), 49);  // Max size is buffer_size - 1
    EXPECT_STREQ(str.c_str( ), "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB");
}

TEST (StaticStringAdapterTest, AssignExceptionStrategy) {
    char                                                 buffer[5];
    static_string_adapter<BoundCheckStrategy::Exception> str(buffer, sizeof(buffer));

    // Test assigning a range longer than max_size should throw
    std::string long_source("123456789");
    EXPECT_THROW(str.assign(long_source.begin( ), long_source.end( )), std::overflow_error);

    // Test assigning count larger than max_size should throw
    EXPECT_THROW(str.assign(10, 'X'), std::overflow_error);
}

TEST (StaticStringAdapterTest, AssignAssertStrategy) {
    char                                              buffer[5];
    static_string_adapter<BoundCheckStrategy::Assert> str(buffer, sizeof(buffer));

    // Set strategy to Assert for testing - this will fail assertion in debug mode
#ifdef NDEBUG
    GTEST_SKIP( ) << "Assert strategy tests skipped in release build";
#else
    std::string long_source("123456789");
    EXPECT_DEATH(str.assign(long_source.begin( ), long_source.end( )), "");
#endif
}

TEST (StaticStringTest, AssignAssertStrategy) {
    static_string<5, BoundCheckStrategy::Assert> str;

    // Set strategy to Assert for testing - this will fail assertion in debug mode
#ifdef NDEBUG
    GTEST_SKIP( ) << "Assert strategy tests skipped in release build";
#else
    std::string long_source("123456789");
    EXPECT_DEATH(str.assign(long_source.begin( ), long_source.end( )), "");
#endif
}

TEST (StaticStringAdapterTest, InsertSingleCharacter) {
    std::array<char, 10>  char_array = {'a', 'b', 'c', '\0'};
    static_string_adapter str(char_array.data( ), char_array.size( ), 3);

    str.insert(0, 'x');
    EXPECT_EQ(str.length( ), 4);
    EXPECT_STREQ(str.c_str( ), "xabc");

    str.insert(str.begin( ) + 2, 'y');
    EXPECT_EQ(str.length( ), 5);
    EXPECT_STREQ(str.c_str( ), "xaybc");
}

TEST (StaticStringAdapterTest, MultipleCharacters) {
    char                  char_array[10] {'a', 'b', 'c', '\0'};
    static_string_adapter str(char_array, std::size(char_array), 3);

    // Insert multiple characters at the beginning
    str.insert(0, "xyz");
    EXPECT_EQ(str.length( ), 6);
    EXPECT_STREQ(str.c_str( ), "xyzabc");

    str.insert(2, 3, '-');
    EXPECT_EQ(str.length( ), 9);
    EXPECT_EQ(str.view( ), "xy---zabc");
}

class StaticStringAdapterInsertTest : public Test {
private:
    std::array<char, 24> data_array_;
    size_t               elements_count_ {0};

protected:
    static_string_adapter<BoundCheckStrategy::Exception> adapter {data_array_.data( ), data_array_.size( ), elements_count_};

    void SetUp ( ) override {
        data_array_.fill('\0');
        elements_count_ = 0;
    }
};

TEST_F (StaticStringAdapterInsertTest, N1) {
    // index, count, ch
    using namespace std::literals;
    EXPECT_NO_THROW(adapter.insert(0, 10, 'a'));
    EXPECT_EQ(adapter.size( ), 10);
    EXPECT_EQ(adapter.begin( ) + adapter.size( ), adapter.end( ));
    EXPECT_STREQ(adapter.c_str( ), "aaaaaaaaaa");
    EXPECT_TRUE(std::ranges::equal(adapter, "aaaaaaaaaa"sv));

    EXPECT_NO_THROW(adapter.insert(3, 3, 'x'));
    EXPECT_EQ(adapter.size( ), 13);
    EXPECT_EQ(adapter.begin( ) + adapter.size( ), adapter.end( ));
    EXPECT_STREQ(adapter.c_str( ), "aaaxxxaaaaaaa");
    EXPECT_TRUE(std::ranges::equal(adapter, "aaaxxxaaaaaaa"sv));

    EXPECT_NO_THROW(adapter.insert(13, 2, '-'));
    EXPECT_EQ(adapter.size( ), 15);
    EXPECT_STREQ(adapter.c_str( ), "aaaxxxaaaaaaa--");

#if !defined(NDEBUG)
    EXPECT_EXIT(adapter.insert<BoundCheckStrategy::Assert>(200, 3, 'f'), KilledBySignal(SIGABRT), "");
#endif
    EXPECT_THROW(adapter.insert<BoundCheckStrategy::Exception>(200, 3, 'f'), std::out_of_range);
    EXPECT_NO_THROW(adapter.insert<BoundCheckStrategy::LimitToBound>(200, 3, 'f'));
    EXPECT_STREQ(adapter.c_str( ), "aaaxxxaaaaaaa--fff");
}

TEST_F (StaticStringAdapterInsertTest, N2) {
    // index, cstr
    using namespace std::literals;
    EXPECT_NO_THROW(adapter.insert(0, "lorem"));
    EXPECT_EQ(adapter.size( ), 5);
    EXPECT_EQ(adapter.begin( ) + adapter.size( ), adapter.end( ));
    EXPECT_STREQ(adapter.c_str( ), "lorem");
    EXPECT_TRUE(std::ranges::equal(adapter, "lorem"sv));

    EXPECT_NO_THROW(adapter.insert(5, " ipsum"));
    EXPECT_EQ(adapter.size( ), 11);
    EXPECT_STREQ(adapter.c_str( ), "lorem ipsum");
}
}  // namespace wbr
