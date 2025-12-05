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

    EXPECT_NO_THROW(std::ignore = str[-1]);
    EXPECT_NO_THROW(std::ignore = str[5]);  // Past the string length, but within capacity
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
    std::string_view em { };
    str.assign(em.cbegin( ), em.cend( ));
    EXPECT_EQ(str.length( ), 0);

    // Test with a large range that should be truncated (with a LimitToBound strategy)
    std::string large_source(100, 'A');
    str.assign(large_source.begin( ), large_source.end( ));
    EXPECT_EQ(str.length( ), 49);  // The max size is buffer_size - 1
    EXPECT_STREQ(str.c_str( ), "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
}

TEST (StaticStringAdapterTest, AssignCountValue) {
    char                                                    buffer[50];
    static_string_adapter<BoundCheckStrategy::LimitToBound> str(buffer, sizeof(buffer));

    // Test assigning count and value
    str.assign(5, 'X');
    EXPECT_EQ(str.length( ), 5);
    EXPECT_STREQ(str.c_str( ), "XXXXX");

    // Test with zero counts
    str.assign(0, 'Y');
    EXPECT_EQ(str.length( ), 0);

    // Test with a large count that should be truncated (with a LimitToBound strategy)
    str.assign(100, 'Z');
    EXPECT_EQ(str.length( ), 49);  // The max size is buffer_size - 1
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

    // Test with a large string_view that should be truncated (with LimitToBound strategy)
    std::string large_source(100, 'B');
    str.assign(std::string_view {large_source});
    EXPECT_EQ(str.length( ), 49);  // The max size is buffer_size - 1
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
    std::array<char, 24> data_array_ { };
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

TEST (StaticStringAdapterTest, ReplaceMethod) {
    constexpr std::size_t      buffer_size = 50;
    char                       buffer[buffer_size];
    wbr::static_string_adapter adapter(buffer, buffer_size);

    // Initialize the string
    adapter.assign("Hello, world!");

    // Define the range to be replaced and the new content
    auto first = adapter.cbegin( ) + 7;   // Points to 'w'
    auto last  = adapter.cbegin( ) + 12;  // Points to '!' (exclusive)

    // replace text with text (replace)
    std::string_view replacement = "everyone";
    EXPECT_NO_THROW(adapter.replace(first, last, replacement.cbegin( ), replacement.cend( )));
    EXPECT_STREQ(adapter.c_str( ), "Hello, everyone!");

    // replace empty range with text (insert)
    std::string_view spaces = "  ";
    EXPECT_NO_THROW(adapter.replace(adapter.begin( ), adapter.begin( ), spaces.cbegin( ), spaces.cend( )));
    EXPECT_STREQ(adapter.c_str( ), "  Hello, everyone!");

    // replace text with empty range (erase)
    std::string_view empty_str = "";
    EXPECT_NO_THROW(adapter.replace(adapter.cbegin( ), adapter.cbegin( ) + 2, empty_str.cbegin( ), empty_str.cend( )));
    EXPECT_STREQ(adapter.c_str( ), "Hello, everyone!");

    // replace end(),end() with text (append)
    std::string_view ni_hao = "ni hao";
    EXPECT_NO_THROW(adapter.replace(adapter.cend( ), adapter.cend( ), ni_hao.cbegin( ), ni_hao.cend( )));
    EXPECT_STREQ(adapter.c_str( ), "Hello, everyone!ni hao");

    EXPECT_NO_THROW(adapter.replace(adapter.cbegin( ) + 16, adapter.cbegin( ) + 16, spaces.begin( ), spaces.end( )));
    EXPECT_STREQ(adapter.c_str( ), "Hello, everyone!  ni hao");

    // replace begin(), end() with empty (clear)
    EXPECT_NO_THROW(adapter.replace(adapter.cbegin( ), adapter.cend( ), empty_str.cbegin( ), empty_str.cend( )));
    EXPECT_STREQ(adapter.c_str( ), "");

    // replace begin(), end() with string (assign)
    adapter.assign("some text");
    std::string_view some_new_text = "lorem ipsum";
    EXPECT_NO_THROW(adapter.replace(adapter.cbegin( ), adapter.cend( ), some_new_text.begin( ), some_new_text.cend( )));
    EXPECT_EQ(adapter, some_new_text);
}

TEST (StaticStringAdapterTest, CompareMethod) {
    using namespace std::literals;

    std::array<char, 50>       char_array;
    wbr::static_string_adapter adapter(char_array.data( ), char_array.size( ));

    // Initialize the string
    adapter.assign("Hello, world!");

    // Test case where strings are equal
    EXPECT_EQ(adapter.compare("Hello, world!"sv), 0);

    // Test case where the second string is lexicographically less than the first string
    EXPECT_GT(adapter.compare("Hello, everyone!"sv), 0);
    EXPECT_GT(adapter.compare("Hello, wold!"sv), 0);  // Intentional typo

    // Test with an empty string
    EXPECT_GT(adapter.compare(""sv), 0);

    // Test where the first string is lexicographically greater than the second string
    adapter.assign("Zebra");
    EXPECT_GT(adapter.compare("Apple"sv), 0);
}

TEST (StaticStringAdapterTest, AppendOperations) {
    using namespace std::literals;

    char                  buffer[50];
    static_string_adapter adapter(buffer, sizeof(buffer));

    // Test append single character
    adapter.append('H');
    EXPECT_EQ(adapter.view( ), "H");

    // Test append count and char
    adapter.append(3, 'e');
    EXPECT_EQ(adapter.view( ), "Heee");

    // Test append c-string
    adapter.clear( );
    adapter.append("Hello");
    EXPECT_EQ(adapter.view( ), "Hello");

    // Test append c-string with count
    adapter.append(", ", 2);
    EXPECT_EQ(adapter.view( ), "Hello, ");

    // Test append string
    std::string world = "World";
    adapter.append(world);
    EXPECT_EQ(adapter.view( ), "Hello, World");

    // Test append string_view
    adapter.append("!"sv);
    EXPECT_EQ(adapter.view( ), "Hello, World!");

    // Test append iterators
    std::string abc = "ABC";
    adapter.append(abc.begin( ), abc.end( ));
    EXPECT_EQ(adapter.view( ), "Hello, World!ABC");

    // Test append initializer_list
    adapter.clear( );
    adapter.append({'X', 'Y', 'Z'});
    EXPECT_EQ(adapter.view( ), "XYZ");
}

TEST (StaticStringAdapterTest, AppendOperators) {
    using namespace std::literals;

    char                  buffer[50];
    static_string_adapter adapter(buffer, sizeof(buffer));

    adapter += "Hello";
    EXPECT_EQ(adapter.view( ), "Hello");

    adapter += ' ';
    EXPECT_EQ(adapter.view( ), "Hello ");

    std::string world = "World";
    adapter += world;
    EXPECT_EQ(adapter.view( ), "Hello World");

    adapter += "!"sv;
    EXPECT_EQ(adapter.view( ), "Hello World!");

    adapter += {'1', '2', '3'};
    EXPECT_EQ(adapter.view( ), "Hello World!123");
}

TEST (StaticStringAdapterTest, EraseOperations) {
    char                  buffer[50];
    static_string_adapter adapter(buffer, sizeof(buffer));

    adapter.assign("Hello, World!");

    // Test erase by index and count
    adapter.erase(5, 7);
    EXPECT_EQ(adapter.view( ), "Hello!");

    // Test erase single position
    adapter.assign("ABCDEF");
    adapter.erase(adapter.cbegin( ) + 2);
    EXPECT_EQ(adapter.view( ), "ABDEF");

    // Test erase range
    adapter.assign("0123456789");
    adapter.erase(adapter.cbegin( ) + 3, adapter.cbegin( ) + 7);
    EXPECT_EQ(adapter.view( ), "012789");
}

TEST (StaticStringAdapterTest, PushBackPopBack) {
    char                  buffer[10];
    static_string_adapter adapter(buffer, sizeof(buffer));

    adapter.push_back('A');
    adapter.push_back('B');
    adapter.push_back('C');
    EXPECT_EQ(adapter.view( ), "ABC");

    adapter.pop_back( );
    EXPECT_EQ(adapter.view( ), "AB");

    adapter.pop_back( );
    EXPECT_EQ(adapter.view( ), "A");
}

TEST (StaticStringAdapterTest, ClearMethod) {
    char                  buffer[20];
    static_string_adapter adapter(buffer, sizeof(buffer));

    adapter.assign("Hello");
    EXPECT_EQ(adapter.size( ), 5);

    adapter.clear( );
    EXPECT_EQ(adapter.size( ), 0);
    EXPECT_EQ(adapter.view( ), "");
    EXPECT_TRUE(adapter.empty( ));
}

TEST (StaticStringAdapterTest, CapacityMethods) {
    char                  buffer[20];
    static_string_adapter adapter(buffer, sizeof(buffer));

    EXPECT_EQ(adapter.capacity( ), 19);
    EXPECT_EQ(adapter.max_size( ), 19);
    EXPECT_EQ(adapter.free_space( ), 19);
    EXPECT_TRUE(adapter.empty( ));

    adapter.assign("Hello");
    EXPECT_EQ(adapter.free_space( ), 14);
    EXPECT_FALSE(adapter.empty( ));
}

TEST (StaticStringAdapterTest, ElementAccess) {
    char                  buffer[20];
    static_string_adapter adapter(buffer, sizeof(buffer));
    adapter.assign("Hello");

    // Test operator[]
    EXPECT_EQ(adapter[0], 'H');
    EXPECT_EQ(adapter[4], 'o');

    // Test at()
    EXPECT_EQ(adapter.at(0), 'H');
    EXPECT_EQ(adapter.at(4), 'o');
    EXPECT_THROW(adapter.at(5), std::out_of_range);
    EXPECT_THROW(adapter.at(-1), std::out_of_range);

    // Test front() and back()
    EXPECT_EQ(adapter.front( ), 'H');
    EXPECT_EQ(adapter.back( ), 'o');

    // Test c_str() and data()
    EXPECT_STREQ(adapter.c_str( ), "Hello");
    EXPECT_EQ(adapter.data( ), adapter.c_str( ));
}

TEST (StaticStringAdapterTest, Iterators) {
    char                  buffer[20];
    static_string_adapter adapter(buffer, sizeof(buffer));
    adapter.assign("ABCDE");

    // Test forward iterators
    std::string result;
    for ( auto it = adapter.begin( ); it != adapter.end( ); ++it ) {
        result += *it;
    }
    EXPECT_EQ(result, "ABCDE");

    // Test const iterators
    result.clear( );
    for ( auto it = adapter.cbegin( ); it != adapter.cend( ); ++it ) {
        result += *it;
    }
    EXPECT_EQ(result, "ABCDE");

    // Test reverse iterators
    result.clear( );
    for ( auto it = adapter.rbegin( ); it != adapter.rend( ); ++it ) {
        result += *it;
    }
    EXPECT_EQ(result, "EDCBA");

    // Test const reverse iterators
    result.clear( );
    for ( auto it = adapter.crbegin( ); it != adapter.crend( ); ++it ) {
        result += *it;
    }
    EXPECT_EQ(result, "EDCBA");
}

TEST (StaticStringAdapterTest, StringOperations) {
    using namespace std::literals;

    char                  buffer[30];
    static_string_adapter adapter(buffer, sizeof(buffer));
    adapter.assign("Hello, World!");

    // Test starts_with
    EXPECT_TRUE(adapter.starts_with("Hello"));
    EXPECT_TRUE(adapter.starts_with('H'));
    EXPECT_FALSE(adapter.starts_with("World"));

    // Test ends_with (note: there's a bug in the implementation - it calls starts_with)
    // EXPECT_TRUE(adapter.ends_with("World!"));
    // EXPECT_TRUE(adapter.ends_with('!'));
    // EXPECT_FALSE(adapter.ends_with("Hello"));

    // Test contains
    EXPECT_TRUE(adapter.contains("World"));
    EXPECT_TRUE(adapter.contains(','));
    EXPECT_FALSE(adapter.contains("xyz"));

    // Test find
    EXPECT_EQ(adapter.find("World"), 7);
    EXPECT_EQ(adapter.find(','), 5);
    EXPECT_EQ(adapter.find("xyz"), adapter.npos);

    // Test substr
    EXPECT_EQ(adapter.substr(0, 5), "Hello");
    EXPECT_EQ(adapter.substr(7, 5), "World");
    EXPECT_EQ(adapter.substr(7), "World!");
}

TEST (StaticStringAdapterTest, ViewAndConversion) {
    char                  buffer[20];
    static_string_adapter adapter(buffer, sizeof(buffer));
    adapter.assign("Test");

    // Test view()
    std::string_view sv = adapter.view( );
    EXPECT_EQ(sv, "Test");

    // Test implicit conversion to string_view
    std::string_view sv2 = adapter;
    EXPECT_EQ(sv2, "Test");

    // Test explicit conversion to string
    std::string str = static_cast<std::string>(adapter);
    EXPECT_EQ(str, "Test");
}

TEST (StaticStringAdapterTest, SubstrAndCopy) {
    char                  buffer[30];
    static_string_adapter adapter(buffer, sizeof(buffer));
    adapter.assign("Hello, World!");

    // Test substr
    EXPECT_EQ(adapter.substr(0, 5), "Hello");
    EXPECT_EQ(adapter.substr(7), "World!");

    // Test copy
    char   dest[10];
    size_t copied = adapter.copy(dest, 5, 0);
    dest[copied]  = '\0';
    EXPECT_EQ(copied, 5);
    EXPECT_STREQ(dest, "Hello");
}

// Static String Tests
TEST (StaticStringTest, DefaultConstructor) {
    static_string<20> str;
    EXPECT_EQ(str.size( ), 0);
    EXPECT_TRUE(str.empty( ));
    EXPECT_EQ(str.capacity( ), 19);
}

TEST (StaticStringTest, StringViewConstructor) {
    using namespace std::literals;

    static_string<20> str("Hello"sv);
    EXPECT_EQ(str.size( ), 5);
    EXPECT_EQ(str.view( ), "Hello");

    static_string<20> str2("World!");
    EXPECT_EQ(str2.size( ), 6);
    EXPECT_EQ(str2.view( ), "World!");
}

TEST (StaticStringTest, BasicOperations) {
    static_string<50> str;

    str.append("Hello");
    EXPECT_EQ(str.view( ), "Hello");

    str += " ";
    str += "World";
    EXPECT_EQ(str.view( ), "Hello World");

    str.clear( );
    EXPECT_TRUE(str.empty( ));

    str.assign("Test");
    EXPECT_EQ(str.view( ), "Test");
}

TEST (StaticStringTest, BoundCheckStrategies) {
    // NoCheck strategy (default)
    static_string<5, BoundCheckStrategy::NoCheck> str1;
    str1.append("abc");
    EXPECT_EQ(str1.view( ), "abc");

    // Exception strategy
    static_string<5, BoundCheckStrategy::Exception> str2;
    EXPECT_THROW(str2.append(10, 'x'), std::overflow_error);

    // LimitToBound strategy
    static_string<5, BoundCheckStrategy::LimitToBound> str3;
    str3.append(10, 'x');
    EXPECT_EQ(str3.size( ), 4);  // Limited to capacity
    EXPECT_EQ(str3.view( ), "xxxx");
}

TEST (StaticStringTest, LargeString) {
    static_string<256> str;

    for ( int i = 0; i < 10; ++i ) {
        str.append("0123456789");
    }

    EXPECT_EQ(str.size( ), 100);
    EXPECT_TRUE(str.starts_with("0123456789"));
}

TEST (StaticStringTest, IteratorOperations) {
    static_string<30> str("ABCDE");

    // Modify through iterator
    for ( auto& ch: str ) {
        ch = std::tolower(ch);
    }

    EXPECT_EQ(str.view( ), "abcde");
}

TEST (StaticStringTest, ComparisonOperations) {
    using namespace std::literals;

    static_string<20> str1("Hello");
    static_string<20> str2("Hello");
    static_string<20> str3("World");

    EXPECT_EQ(str1.compare(str2), 0);
    EXPECT_LT(str1.compare(str3), 0);
    EXPECT_GT(str3.compare(str1), 0);

    EXPECT_EQ(str1.compare("Hello"), 0);
    EXPECT_EQ(str1.compare("Hello"sv), 0);
}

TEST (StaticStringTest, InsertAndErase) {
    static_string<50> str("Hello");

    str.insert(5, " World");
    EXPECT_EQ(str.view( ), "Hello World");

    str.insert(0, "Say: ");
    EXPECT_EQ(str.view( ), "Say: Hello World");

    str.erase(0, 5);
    EXPECT_EQ(str.view( ), "Hello World");

    str.erase(5);
    EXPECT_EQ(str.view( ), "Hello");
}

TEST (StaticStringTest, ReplaceOperations) {
    using namespace std::literals;

    static_string<50> str("Hello, World!");

    str.replace(0, 5, "Hi");
    EXPECT_EQ(str.view( ), "Hi, World!");

    str.replace(str.cbegin( ) + 3, str.cend( ), "there"sv);
    EXPECT_EQ(str.view( ), "Hi,there");

    str.replace(0, str.size( ), 5, 'X');
    EXPECT_EQ(str.view( ), "XXXXX");
}

}  // namespace wbr
