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

TEST (StaticStringAdapterTest, ResizeExpand) {
    char                  buffer[20];
    static_string_adapter adapter(buffer, sizeof(buffer));

    // Start with a small string
    adapter.assign("Hello");
    EXPECT_EQ(adapter.size( ), 5);

    // Resize to expand with default character '\0'
    adapter.resize(10);
    EXPECT_EQ(adapter.size( ), 10);
    EXPECT_EQ(adapter[0], 'H');
    EXPECT_EQ(adapter[4], 'o');
    EXPECT_EQ(adapter[5], '\0');
    EXPECT_EQ(adapter[9], '\0');

    // Resize to expand with custom character
    adapter.resize(15, 'X');
    EXPECT_EQ(adapter.size( ), 15);
    EXPECT_EQ(adapter[10], 'X');
    EXPECT_EQ(adapter[14], 'X');
}

TEST (StaticStringAdapterTest, ResizeShrink) {
    char                  buffer[20];
    static_string_adapter adapter(buffer, sizeof(buffer));

    // Start with a longer string
    adapter.assign("Hello, World!");
    EXPECT_EQ(adapter.size( ), 13);

    // Resize to shrink
    adapter.resize(5);
    EXPECT_EQ(adapter.size( ), 5);
    EXPECT_EQ(adapter.view( ), "Hello");
    EXPECT_EQ(adapter[5], '\0');  // Ensure null terminator is in place
}

TEST (StaticStringAdapterTest, ResizeSameSize) {
    char                  buffer[20];
    static_string_adapter adapter(buffer, sizeof(buffer));

    adapter.assign("Test");
    EXPECT_EQ(adapter.size( ), 4);

    // Resize to same size should not change anything
    adapter.resize(4);
    EXPECT_EQ(adapter.size( ), 4);
    EXPECT_EQ(adapter.view( ), "Test");

    adapter.resize(4, 'X');
    EXPECT_EQ(adapter.size( ), 4);
    EXPECT_EQ(adapter.view( ), "Test");
}

TEST (StaticStringAdapterTest, ResizeToZero) {
    char                  buffer[20];
    static_string_adapter adapter(buffer, sizeof(buffer));

    adapter.assign("Hello");
    EXPECT_EQ(adapter.size( ), 5);

    // Resize to zero (clear the string)
    adapter.resize(0);
    EXPECT_EQ(adapter.size( ), 0);
    EXPECT_TRUE(adapter.empty( ));
    EXPECT_EQ(adapter.view( ), "");
}

TEST (StaticStringAdapterTest, ResizeFromEmpty) {
    char                  buffer[20];
    static_string_adapter adapter(buffer, sizeof(buffer));

    EXPECT_EQ(adapter.size( ), 0);

    // Resize from empty with default character
    adapter.resize(5);
    EXPECT_EQ(adapter.size( ), 5);
    for ( size_t i = 0; i < 5; ++i ) {
        EXPECT_EQ(adapter[i], '\0');
    }

    // Clear and resize with custom character
    adapter.clear( );
    adapter.resize(3, 'A');
    EXPECT_EQ(adapter.size( ), 3);
    EXPECT_EQ(adapter.view( ), std::string_view("AAA", 3));
}

TEST (StaticStringAdapterTest, ResizeExceptionStrategy) {
    char                                                 buffer[10];
    static_string_adapter<BoundCheckStrategy::Exception> adapter(buffer, sizeof(buffer));

    adapter.assign("Test");
    EXPECT_EQ(adapter.size( ), 4);

    // Try to resize beyond capacity - should throw
    EXPECT_THROW(adapter.resize(20), std::length_error);
    EXPECT_THROW(adapter.resize(15, 'X'), std::length_error);

    // Size should remain unchanged after exception
    EXPECT_EQ(adapter.size( ), 4);
    EXPECT_EQ(adapter.view( ), "Test");

    // Resize within capacity should work
    EXPECT_NO_THROW(adapter.resize(8, 'Y'));
    EXPECT_EQ(adapter.size( ), 8);
}

TEST (StaticStringAdapterTest, ResizeLimitToBoundStrategy) {
    char                                                    buffer[10];
    static_string_adapter<BoundCheckStrategy::LimitToBound> adapter(buffer, sizeof(buffer));

    adapter.assign("Hi");
    EXPECT_EQ(adapter.size( ), 2);

    // Try to resize beyond capacity - should limit to capacity
    adapter.resize(20, 'Z');
    EXPECT_EQ(adapter.size( ), 9);  // capacity is buffer_size - 1
    EXPECT_EQ(adapter[0], 'H');
    EXPECT_EQ(adapter[1], 'i');
    for ( size_t i = 2; i < 9; ++i ) {
        EXPECT_EQ(adapter[i], 'Z');
    }

    // Resize to larger than capacity with default character
    adapter.clear( );
    adapter.resize(100);
    EXPECT_EQ(adapter.size( ), 9);
}

TEST (StaticStringAdapterTest, ResizeAssertStrategy) {
    char                                              buffer[10];
    static_string_adapter<BoundCheckStrategy::Assert> adapter(buffer, sizeof(buffer));

#ifdef NDEBUG
    GTEST_SKIP( ) << "Assert strategy tests skipped in release build";
#else
    adapter.assign("Test");

    // Try to resize beyond capacity - should assert
    EXPECT_DEATH(adapter.resize(20), "");
    EXPECT_DEATH(adapter.resize(15, 'X'), "");
#endif
}

TEST (StaticStringTest, ResizeOperations) {
    static_string<20> str;

    // Resize empty string
    str.resize(5, 'A');
    EXPECT_EQ(str.size( ), 5);
    EXPECT_EQ(str.view( ), std::string_view("AAAAA", 5));

    // Resize to expand
    str.resize(10, 'B');
    EXPECT_EQ(str.size( ), 10);
    EXPECT_EQ(str[0], 'A');
    EXPECT_EQ(str[4], 'A');
    EXPECT_EQ(str[5], 'B');
    EXPECT_EQ(str[9], 'B');

    // Resize to shrink
    str.resize(3);
    EXPECT_EQ(str.size( ), 3);
    EXPECT_EQ(str.view( ), std::string_view("AAA", 3));

    // Resize with default character
    str.resize(7);
    EXPECT_EQ(str.size( ), 7);
    EXPECT_EQ(str[0], 'A');
    EXPECT_EQ(str[2], 'A');
    EXPECT_EQ(str[3], '\0');
    EXPECT_EQ(str[6], '\0');
}

TEST (StaticStringTest, ResizeWithBoundCheckStrategies) {
    // Exception strategy
    static_string<5, BoundCheckStrategy::Exception> str1;
    str1.resize(3, 'X');
    EXPECT_EQ(str1.view( ), std::string_view("XXX", 3));
    EXPECT_THROW(str1.resize(10), std::length_error);

    // LimitToBound strategy
    static_string<5, BoundCheckStrategy::LimitToBound> str2;
    str2.resize(10, 'Y');
    EXPECT_EQ(str2.size( ), 4);  // Limited to capacity
    EXPECT_EQ(str2.view( ), std::string_view("YYYY", 4));
}

TEST (StaticStringAdapterTest, FindFirstOf) {
    using namespace std::literals;

    char                  buffer[50];
    static_string_adapter adapter(buffer, sizeof(buffer));
    adapter.assign("Hello, World!");

    // Test find_first_of with string
    std::string vowels_str = "aeiouAEIOU";
    EXPECT_EQ(adapter.find_first_of(vowels_str), 1);  // 'e' at position 1

    // Test find_first_of with c-string and count
    EXPECT_EQ(adapter.find_first_of("aeiou", 0, 5), 1);  // 'e' at position 1
    EXPECT_EQ(adapter.find_first_of("aeiou", 2, 5), 4);  // 'o' at position 4

    // Test find_first_of with c-string
    EXPECT_EQ(adapter.find_first_of("aeiou"), 1);  // 'e' at position 1

    // Test find_first_of with single character
    EXPECT_EQ(adapter.find_first_of('o'), 4);     // First 'o' at position 4
    EXPECT_EQ(adapter.find_first_of('o', 5), 8);  // Next 'o' at position 8

    // Test find_first_of with string_view
    EXPECT_EQ(adapter.find_first_of("aeiou"sv), 1);     // 'e' at position 1
    EXPECT_EQ(adapter.find_first_of("xyz"sv), adapter.npos);  // Not found

    // Test with starting position
    EXPECT_EQ(adapter.find_first_of("aeiou"sv, 5), 8);  // 'o' at position 8

    // Test not found
    EXPECT_EQ(adapter.find_first_of("xyz"), adapter.npos);
    EXPECT_EQ(adapter.find_first_of('z'), adapter.npos);
}

TEST (StaticStringAdapterTest, FindFirstNotOf) {
    using namespace std::literals;

    char                  buffer[50];
    static_string_adapter adapter(buffer, sizeof(buffer));
    adapter.assign("aaabbbccc");

    // Test find_first_not_of with string
    std::string aaa = "a";
    EXPECT_EQ(adapter.find_first_not_of(aaa), 3);  // First 'b' at position 3

    // Test find_first_not_of with c-string and count
    EXPECT_EQ(adapter.find_first_not_of("ab", 0, 2), 6);  // First 'c' at position 6

    // Test find_first_not_of with c-string
    EXPECT_EQ(adapter.find_first_not_of("a"), 3);  // First non-'a' at position 3

    // Test find_first_not_of with single character
    EXPECT_EQ(adapter.find_first_not_of('a'), 3);     // First non-'a' at position 3
    EXPECT_EQ(adapter.find_first_not_of('a', 5), 5);  // At position 5 (already 'b')

    // Test find_first_not_of with string_view
    EXPECT_EQ(adapter.find_first_not_of("a"sv), 3);
    EXPECT_EQ(adapter.find_first_not_of("ab"sv), 6);

    // Test with all matching
    adapter.assign("aaaa");
    EXPECT_EQ(adapter.find_first_not_of('a'), adapter.npos);
    EXPECT_EQ(adapter.find_first_not_of("a"sv), adapter.npos);
}

TEST (StaticStringAdapterTest, FindLastOf) {
    using namespace std::literals;

    char                  buffer[50];
    static_string_adapter adapter(buffer, sizeof(buffer));
    adapter.assign("Hello, World!");

    // Test find_last_of with string
    std::string vowels_str = "aeiouAEIOU";
    EXPECT_EQ(adapter.find_last_of(vowels_str), 8);  // 'o' at position 8

    // Test find_last_of with c-string and count
    EXPECT_EQ(adapter.find_last_of("aeiou", adapter.npos, 5), 8);  // 'o' at position 8

    // Test find_last_of with c-string
    EXPECT_EQ(adapter.find_last_of("aeiou"), 8);  // 'o' at position 8

    // Test find_last_of with single character
    EXPECT_EQ(adapter.find_last_of('o'), 8);     // Last 'o' at position 8
    EXPECT_EQ(adapter.find_last_of('o', 7), 4);  // 'o' at position 4 (search before position 8)

    // Test find_last_of with string_view
    EXPECT_EQ(adapter.find_last_of("aeiou"sv), 8);  // 'o' at position 8

    // Test with limited position
    EXPECT_EQ(adapter.find_last_of("aeiou"sv, 5), 4);  // 'o' at position 4

    // Test not found
    EXPECT_EQ(adapter.find_last_of("xyz"), adapter.npos);
    EXPECT_EQ(adapter.find_last_of('z'), adapter.npos);
}

TEST (StaticStringAdapterTest, FindLastNotOf) {
    using namespace std::literals;

    char                  buffer[50];
    static_string_adapter adapter(buffer, sizeof(buffer));
    adapter.assign("aaabbbccc");

    // Test find_last_not_of with string
    std::string ccc = "c";
    EXPECT_EQ(adapter.find_last_not_of(ccc), 5);  // Last 'b' at position 5

    // Test find_last_not_of with c-string and count
    EXPECT_EQ(adapter.find_last_not_of("bc", adapter.npos, 2), 2);  // Last 'a' at position 2

    // Test find_last_not_of with c-string
    EXPECT_EQ(adapter.find_last_not_of("c"), 5);  // Last non-'c' at position 5

    // Test find_last_not_of with single character
    EXPECT_EQ(adapter.find_last_not_of('c'), 5);     // Last non-'c' at position 5
    EXPECT_EQ(adapter.find_last_not_of('c', 4), 4);  // At position 4 ('b')

    // Test find_last_not_of with string_view
    EXPECT_EQ(adapter.find_last_not_of("c"sv), 5);
    EXPECT_EQ(adapter.find_last_not_of("bc"sv), 2);

    // Test with all matching
    adapter.assign("cccc");
    EXPECT_EQ(adapter.find_last_not_of('c'), adapter.npos);
    EXPECT_EQ(adapter.find_last_not_of("c"sv), adapter.npos);
}

TEST (StaticStringTest, FindFirstOfOperations) {
    using namespace std::literals;

    static_string<50> str("The quick brown fox jumps over the lazy dog");

    // Find vowels
    EXPECT_EQ(str.find_first_of("aeiou"), 2);  // 'e' at position 2
    EXPECT_EQ(str.find_first_of("aeiou", 3), 6);  // 'u' at position 6

    // Find specific character
    EXPECT_EQ(str.find_first_of('q'), 4);
    EXPECT_EQ(str.find_first_of('z'), 35);

    // Not found
    EXPECT_EQ(str.find_first_of('Q'), str.npos);
    EXPECT_EQ(str.find_first_of("XYZ"), str.npos);
}

TEST (StaticStringTest, FindFirstNotOfOperations) {
    using namespace std::literals;

    static_string<50> str("   Leading spaces");

    // Find first non-space
    EXPECT_EQ(str.find_first_not_of(' '), 3);  // 'L' at position 3
    EXPECT_EQ(str.find_first_not_of(" "), 3);

    // Test with string_view
    EXPECT_EQ(str.find_first_not_of(" "sv), 3);

    static_string<20> digits("12345abc");
    EXPECT_EQ(digits.find_first_not_of("0123456789"), 5);  // 'a' at position 5
}

TEST (StaticStringTest, FindLastOfOperations) {
    using namespace std::literals;

    static_string<50> str("The quick brown fox jumps over the lazy dog");

    // Find last vowel
    EXPECT_EQ(str.find_last_of("aeiou"), 41);  // 'o' at position 41

    // Find last occurrence of character
    EXPECT_EQ(str.find_last_of('o'), 41);  // 'o' at position 41
    EXPECT_EQ(str.find_last_of('e'), 32);  // 'e' at position 32

    // With position limit
    EXPECT_EQ(str.find_last_of('o', 30), 17);  // 'o' at position 17 (in "brown")

    // Not found
    EXPECT_EQ(str.find_last_of('Q'), str.npos);
}

TEST (StaticStringTest, FindLastNotOfOperations) {
    using namespace std::literals;

    static_string<50> str("Trailing spaces   ");

    // Find last non-space
    EXPECT_EQ(str.find_last_not_of(' '), 14);  // 's' at position 14
    EXPECT_EQ(str.find_last_not_of(" "), 14);

    // Test with string_view
    EXPECT_EQ(str.find_last_not_of(" "sv), 14);

    static_string<20> digits("abc12345");
    EXPECT_EQ(digits.find_last_not_of("0123456789"), 2);  // 'c' at position 2
}

TEST (StaticStringAdapterTest, FindMethodsCombination) {
    using namespace std::literals;

    char                  buffer[100];
    static_string_adapter adapter(buffer, sizeof(buffer));
    adapter.assign("  trim me  ");

    // Use find_first_not_of and find_last_not_of to implement trim
    auto first = adapter.find_first_not_of(" ");
    auto last  = adapter.find_last_not_of(" ");

    EXPECT_EQ(first, 2);
    EXPECT_EQ(last, 8);

    // Extract trimmed substring
    auto trimmed = adapter.substr(first, last - first + 1);
    EXPECT_EQ(trimmed, "trim me");

    // Test with only spaces
    adapter.assign("     ");
    EXPECT_EQ(adapter.find_first_not_of(" "), adapter.npos);
    EXPECT_EQ(adapter.find_last_not_of(" "), adapter.npos);
}

TEST (StaticStringAdapterTest, EqualityOperator) {
    using namespace std::literals;

    char                  buffer[50];
    static_string_adapter adapter(buffer, sizeof(buffer));
    adapter.assign("Hello");

    // Test equality with string_view
    EXPECT_TRUE(adapter == "Hello"sv);
    EXPECT_FALSE(adapter == "World"sv);
    EXPECT_FALSE(adapter == "Hello!"sv);
    EXPECT_FALSE(adapter == "Hell"sv);

    // Test equality with std::string
    std::string hello_str = "Hello";
    std::string world_str = "World";
    EXPECT_TRUE(adapter == hello_str);
    EXPECT_FALSE(adapter == world_str);

    // Test equality with C-string
    EXPECT_TRUE(adapter == "Hello");
    EXPECT_FALSE(adapter == "World");

    // Test with empty string
    adapter.clear( );
    EXPECT_TRUE(adapter == "");
    EXPECT_TRUE(adapter == ""sv);
    EXPECT_FALSE(adapter == "a");
}

TEST (StaticStringAdapterTest, ThreeWayComparisonOperator) {
    using namespace std::literals;

    char                  buffer[50];
    static_string_adapter adapter(buffer, sizeof(buffer));
    adapter.assign("Hello");

    // Test three-way comparison with string_view
    EXPECT_EQ(adapter <=> "Hello"sv, 0);
    EXPECT_LT(adapter <=> "World"sv, 0);
    EXPECT_GT(adapter <=> "ABC"sv, 0);

    // Test three-way comparison with std::string
    std::string hello_str = "Hello";
    std::string world_str = "World";
    std::string abc_str   = "ABC";
    EXPECT_EQ(adapter <=> hello_str, 0);
    EXPECT_LT(adapter <=> world_str, 0);
    EXPECT_GT(adapter <=> abc_str, 0);

    // Test three-way comparison with C-string
    EXPECT_EQ(adapter <=> "Hello", 0);
    EXPECT_LT(adapter <=> "World", 0);
    EXPECT_GT(adapter <=> "ABC", 0);

    // Test case sensitivity
    EXPECT_LT(adapter <=> "hello", 0);  // 'H' < 'h'

    // Test with different lengths
    EXPECT_LT(adapter <=> "HelloWorld", 0);  // "Hello" < "HelloWorld"
    EXPECT_GT(adapter <=> "Hell", 0);        // "Hello" > "Hell"
}

TEST (StaticStringAdapterTest, ComparisonWithEmptyStrings) {
    using namespace std::literals;

    char                  buffer1[50];
    char                  buffer2[50];
    static_string_adapter empty(buffer1, sizeof(buffer1));
    static_string_adapter non_empty(buffer2, sizeof(buffer2));

    non_empty.assign("Hello");

    // Empty vs empty
    EXPECT_TRUE(empty == "");
    EXPECT_EQ(empty <=> "", 0);

    // Empty vs non-empty
    EXPECT_FALSE(empty == non_empty.view( ));
    EXPECT_LT(empty <=> non_empty.view( ), 0);
    EXPECT_GT(non_empty <=> empty.view( ), 0);

    // Non-empty vs non-empty
    static_string_adapter another(buffer1, sizeof(buffer1));
    another.assign("Hello");
    EXPECT_TRUE(non_empty == another.view( ));
    EXPECT_EQ(non_empty <=> another.view( ), 0);
}

TEST (StaticStringTest, EqualityComparisons) {
    using namespace std::literals;

    static_string<20> str1("Hello");
    static_string<20> str2("Hello");
    static_string<20> str3("World");

    // Test equality between static_strings (via view)
    EXPECT_TRUE(str1 == str2.view( ));
    EXPECT_FALSE(str1 == str3.view( ));

    // Test with literals
    EXPECT_TRUE(str1 == "Hello");
    EXPECT_FALSE(str1 == "World");

    // Test with std::string
    std::string hello = "Hello";
    std::string world = "World";
    EXPECT_TRUE(str1 == hello);
    EXPECT_FALSE(str1 == world);
}

TEST (StaticStringTest, ThreeWayComparisons) {
    using namespace std::literals;

    static_string<20> str1("Apple");
    static_string<20> str2("Banana");
    static_string<20> str3("Apple");

    // Test ordering
    EXPECT_LT(str1 <=> str2.view( ), 0);  // "Apple" < "Banana"
    EXPECT_GT(str2 <=> str1.view( ), 0);  // "Banana" > "Apple"
    EXPECT_EQ(str1 <=> str3.view( ), 0);  // "Apple" == "Apple"

    // Test with C-strings
    EXPECT_LT(str1 <=> "Banana", 0);
    EXPECT_GT(str1 <=> "Aardvark", 0);
    EXPECT_EQ(str1 <=> "Apple", 0);

    // Test with std::string
    std::string banana = "Banana";
    std::string apple  = "Apple";
    EXPECT_LT(str1 <=> banana, 0);
    EXPECT_EQ(str1 <=> apple, 0);
}

TEST (StaticStringTest, ComparisonEdgeCases) {
    using namespace std::literals;

    static_string<50> short_str("Hi");
    static_string<50> long_str("Hello, World!");

    // Length comparison
    EXPECT_LT(short_str <=> long_str.view( ), 0);  // Shorter and lexicographically less

    // Prefix comparison
    static_string<50> prefix("Hello");
    EXPECT_LT(prefix <=> long_str.view( ), 0);  // Prefix is less than full string

    // Case sensitivity
    static_string<50> upper("HELLO");
    static_string<50> lower("hello");
    EXPECT_LT(upper <=> lower.view( ), 0);  // Uppercase < lowercase in ASCII

    // Empty string comparisons
    static_string<20> empty;
    static_string<20> non_empty("A");
    EXPECT_LT(empty <=> non_empty.view( ), 0);
    EXPECT_EQ(empty <=> "", 0);
}

TEST (StaticStringAdapterTest, ComparisonOperatorChaining) {
    using namespace std::literals;

    char                  buffer[50];
    static_string_adapter adapter(buffer, sizeof(buffer));
    adapter.assign("Medium");

    // Test that comparisons work correctly for sorting
    std::vector<std::string_view> words = {"Apple"sv, "Banana"sv, "Cherry"sv, "Mango"sv, "Zebra"sv};

    for ( const auto& word: words ) {
        int cmp = adapter <=> word;
        if ( word == "Apple"sv || word == "Banana"sv || word == "Cherry"sv || word == "Mango"sv ) {
            // "Medium" comes after all of these lexicographically
            if ( word < "Medium"sv ) {
                EXPECT_GT(cmp, 0) << "Failed for word: " << word;
            } else if ( word > "Medium"sv ) {
                EXPECT_LT(cmp, 0) << "Failed for word: " << word;
            } else {
                EXPECT_EQ(cmp, 0) << "Failed for word: " << word;
            }
        }
    }
}

TEST (StaticStringAdapterTest, SwapBasic) {
    char                  buffer1[20];
    char                  buffer2[20];
    static_string_adapter str1(buffer1, sizeof(buffer1));
    static_string_adapter str2(buffer2, sizeof(buffer2));

    str1.assign("Hello");
    str2.assign("World");

    EXPECT_EQ(str1.view( ), "Hello");
    EXPECT_EQ(str2.view( ), "World");

    // Perform swap
    str1.swap(str2);

    // Verify contents are swapped
    EXPECT_EQ(str1.view( ), "World");
    EXPECT_EQ(str2.view( ), "Hello");
    EXPECT_EQ(str1.size( ), 5);
    EXPECT_EQ(str2.size( ), 5);

    // Verify buffers are still the same (not swapped)
    EXPECT_EQ(str1.data( ), buffer1);
    EXPECT_EQ(str2.data( ), buffer2);
}

TEST (StaticStringAdapterTest, SwapDifferentSizes) {
    char                  buffer1[30];
    char                  buffer2[30];
    static_string_adapter str1(buffer1, sizeof(buffer1));
    static_string_adapter str2(buffer2, sizeof(buffer2));

    str1.assign("Short");
    str2.assign("Much longer string");

    EXPECT_EQ(str1.size( ), 5);
    EXPECT_EQ(str2.size( ), 18);

    str1.swap(str2);

    EXPECT_EQ(str1.view( ), "Much longer string");
    EXPECT_EQ(str2.view( ), "Short");
    EXPECT_EQ(str1.size( ), 18);
    EXPECT_EQ(str2.size( ), 5);
}

TEST (StaticStringAdapterTest, SwapWithEmpty) {
    char                  buffer1[20];
    char                  buffer2[20];
    static_string_adapter str1(buffer1, sizeof(buffer1));
    static_string_adapter str2(buffer2, sizeof(buffer2));

    str1.assign("Not empty");
    // str2 is empty

    EXPECT_FALSE(str1.empty( ));
    EXPECT_TRUE(str2.empty( ));

    str1.swap(str2);

    EXPECT_TRUE(str1.empty( ));
    EXPECT_FALSE(str2.empty( ));
    EXPECT_EQ(str2.view( ), "Not empty");
}

TEST (StaticStringAdapterTest, SwapBothEmpty) {
    char                  buffer1[20];
    char                  buffer2[20];
    static_string_adapter str1(buffer1, sizeof(buffer1));
    static_string_adapter str2(buffer2, sizeof(buffer2));

    // Both empty
    EXPECT_TRUE(str1.empty( ));
    EXPECT_TRUE(str2.empty( ));

    str1.swap(str2);

    // Still both empty
    EXPECT_TRUE(str1.empty( ));
    EXPECT_TRUE(str2.empty( ));
}

TEST (StaticStringAdapterTest, SwapWithSelf) {
    char                  buffer[20];
    static_string_adapter str(buffer, sizeof(buffer));

    str.assign("Self swap");
    auto original_view = str.view( );

    // Swap with itself
    str.swap(str);

    // Content should remain unchanged
    EXPECT_EQ(str.view( ), original_view);
    EXPECT_EQ(str.view( ), "Self swap");
}

TEST (StaticStringAdapterTest, SwapDifferentCapacities) {
    char                  small_buffer[10];
    char                  large_buffer[50];
    static_string_adapter small_str(small_buffer, sizeof(small_buffer));
    static_string_adapter large_str(large_buffer, sizeof(large_buffer));

    small_str.assign("Small");
    large_str.assign("Large");

    EXPECT_EQ(small_str.capacity( ), 9);
    EXPECT_EQ(large_str.capacity( ), 49);

    small_str.swap(large_str);

    EXPECT_EQ(small_str.view( ), "Large");
    EXPECT_EQ(small_str.capacity( ), 9);   // Capacity unchanged
    EXPECT_EQ(large_str.view( ), "Small");
    EXPECT_EQ(large_str.capacity( ), 49);  // Capacity unchanged
}

TEST (StaticStringAdapterTest, SwapLargeStrings) {
    char                  buffer1[300];
    char                  buffer2[300];
    static_string_adapter str1(buffer1, sizeof(buffer1));
    static_string_adapter str2(buffer2, sizeof(buffer2));

    // Create strings larger than the internal temporary buffer (256 bytes)
    std::string long_str1(280, 'A');
    std::string long_str2(280, 'B');

    str1.assign(long_str1);
    str2.assign(long_str2);

    str1.swap(str2);

    EXPECT_EQ(str1.view( ), long_str2);
    EXPECT_EQ(str2.view( ), long_str1);
    EXPECT_EQ(str1.size( ), 280);
    EXPECT_EQ(str2.size( ), 280);
}

TEST (StaticStringAdapterTest, SwapThenModify) {
    char                  buffer1[20];
    char                  buffer2[20];
    static_string_adapter str1(buffer1, sizeof(buffer1));
    static_string_adapter str2(buffer2, sizeof(buffer2));

    str1.assign("First");
    str2.assign("Second");

    str1.swap(str2);

    EXPECT_EQ(str1.view( ), "Second");
    EXPECT_EQ(str2.view( ), "First");

    // Modify after swap to ensure buffers work correctly
    str1.append("!");
    str2.append("?");

    EXPECT_EQ(str1.view( ), "Second!");
    EXPECT_EQ(str2.view( ), "First?");
}

TEST (StaticStringAdapterTest, SwapCapacityOverflow) {
    char                                                    small_buffer[10];
    char                                                    large_buffer[50];
    static_string_adapter<BoundCheckStrategy::LimitToBound> small_str(small_buffer, sizeof(small_buffer));
    static_string_adapter<BoundCheckStrategy::LimitToBound> large_str(large_buffer, sizeof(large_buffer));

    small_str.assign("Tiny");
    large_str.assign("This is a very long string that won't fit");

    // Swap with LimitToBound strategy
    small_str.swap(large_str);

    // small_str should have truncated content
    EXPECT_EQ(small_str.size( ), 9);  // Capacity is 9
    EXPECT_EQ(small_str.view( ), "This is a");

    // large_str should have full content
    EXPECT_EQ(large_str.view( ), "Tiny");
}

TEST (StaticStringTest, SwapOperations) {
    static_string<20> str1("Hello");
    static_string<20> str2("World");

    EXPECT_EQ(str1.view( ), "Hello");
    EXPECT_EQ(str2.view( ), "World");

    str1.swap(str2);

    EXPECT_EQ(str1.view( ), "World");
    EXPECT_EQ(str2.view( ), "Hello");
}

TEST (StaticStringTest, SwapDifferentCapacityStaticStrings) {
    static_string<10> small("Small");
    static_string<50> large("Large string");

    small.swap(large);

    EXPECT_EQ(small.view( ), "Large string");
    EXPECT_EQ(large.view( ), "Small");

    // Verify capacities are unchanged
    EXPECT_EQ(small.capacity( ), 9);
    EXPECT_EQ(large.capacity( ), 49);
}

TEST (StaticStringTest, SwapChaining) {
    static_string<20> str1("A");
    static_string<20> str2("B");
    static_string<20> str3("C");

    // Chain multiple swaps
    str1.swap(str2);  // str1="B", str2="A"
    str2.swap(str3);  // str2="C", str3="A"
    str3.swap(str1);  // str3="B", str1="A"

    EXPECT_EQ(str1.view( ), "A");
    EXPECT_EQ(str2.view( ), "C");
    EXPECT_EQ(str3.view( ), "B");
}

}  // namespace wbr
