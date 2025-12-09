#include "len_string.hxx"

#include <gtest/gtest.h>

using namespace wbr;
using namespace testing;

namespace wbr {
TEST (LenStringAdapterTest, BasicConstruction) {
    char                             buffer[20];
    uint8_t                          len = 0;
    wbr::len_string_adapter<uint8_t> adapter(buffer, sizeof(buffer), len);

    EXPECT_EQ(adapter.size( ), 0);
    EXPECT_EQ(adapter.capacity( ), 20);
    EXPECT_TRUE(adapter.empty( ));
    EXPECT_EQ(len, 0);
}

TEST (LenStringAdapterTest, ConstructionWithInitialLength) {
    char                             buffer[20] = "Hello";
    uint8_t                          len        = 5;
    wbr::len_string_adapter<uint8_t> adapter(buffer, sizeof(buffer), len);

    EXPECT_EQ(adapter.size( ), 5);
    EXPECT_EQ(adapter.view( ), "Hello");
    EXPECT_EQ(len, 5);
}

TEST (LenStringAdapterTest, ArrayConstruction) {
    std::array<char, 20>              arr;
    uint16_t                          len = 0;
    wbr::len_string_adapter<uint16_t> adapter(arr, len);

    EXPECT_EQ(adapter.capacity( ), 20);
    EXPECT_EQ(adapter.size( ), 0);
}

TEST (LenStringAdapterTest, CArrayConstruction) {
    char                             buffer[20];
    uint8_t                          len = 0;
    wbr::len_string_adapter<uint8_t> adapter(buffer, len);

    EXPECT_EQ(adapter.capacity( ), 20);
}

TEST (LenStringAdapterTest, SpanConstruction) {
    char                             buffer[20];
    uint8_t                          len = 0;
    std::span<char>                  sp {buffer, 20};
    wbr::len_string_adapter<uint8_t> adapter(sp, len);

    EXPECT_EQ(adapter.capacity( ), 20);
}

TEST (LenStringAdapterTest, AssignAndAppend) {
    using namespace std::literals;

    char                             buffer[50];
    uint8_t                          len = 0;
    wbr::len_string_adapter<uint8_t> adapter(buffer, sizeof(buffer), len);

    adapter.assign("Hello");
    EXPECT_EQ(adapter.view( ), "Hello");
    EXPECT_EQ(len, 5);
    EXPECT_EQ(adapter.size( ), 5);

    adapter.append(" World");
    EXPECT_EQ(adapter.view( ), "Hello World");
    EXPECT_EQ(len, 11);
    EXPECT_EQ(adapter.size( ), 11);
}

TEST (LenStringAdapterTest, ClearOperation) {
    char                             buffer[20];
    uint8_t                          len = 10;
    wbr::len_string_adapter<uint8_t> adapter(buffer, sizeof(buffer), len);

    adapter.clear( );
    EXPECT_EQ(len, 0);
    EXPECT_EQ(adapter.size( ), 0);
    EXPECT_TRUE(adapter.empty( ));
}

TEST (LenStringAdapterTest, ResizeOperations) {
    char                             buffer[20];
    uint8_t                          len = 0;
    wbr::len_string_adapter<uint8_t> adapter(buffer, sizeof(buffer), len);

    adapter.resize(10, 'A');
    EXPECT_EQ(len, 10);
    EXPECT_EQ(adapter.size( ), 10);
    EXPECT_EQ(adapter.view( ), std::string_view("AAAAAAAAAA", 10));

    adapter.resize(5);
    EXPECT_EQ(len, 5);
    EXPECT_EQ(adapter.size( ), 5);
}

TEST (LenStringAdapterTest, ElementAccess) {
    char                             buffer[20] = "Hello";
    uint8_t                          len        = 5;
    wbr::len_string_adapter<uint8_t> adapter(buffer, sizeof(buffer), len);

    EXPECT_EQ(adapter[0], 'H');
    EXPECT_EQ(adapter[4], 'o');
    EXPECT_EQ(adapter.at(0), 'H');
    EXPECT_EQ(adapter.front( ), 'H');
    EXPECT_EQ(adapter.back( ), 'o');

    EXPECT_THROW(std::ignore = adapter.at(10), std::out_of_range);
}

TEST (LenStringAdapterTest, Iterators) {
    char                             buffer[20] = "ABCDE";
    uint8_t                          len        = 5;
    wbr::len_string_adapter<uint8_t> adapter(buffer, sizeof(buffer), len);

    std::string result;
    for ( auto it = adapter.begin( ); it != adapter.end( ); ++it ) {
        result += *it;
    }
    EXPECT_EQ(result, "ABCDE");

    result.clear( );
    for ( auto it = adapter.rbegin( ); it != adapter.rend( ); ++it ) {
        result += *it;
    }
    EXPECT_EQ(result, "EDCBA");
}

TEST (LenStringAdapterTest, PushPopOperations) {
    char                             buffer[20];
    uint8_t                          len = 0;
    wbr::len_string_adapter<uint8_t> adapter(buffer, sizeof(buffer), len);

    adapter.push_back('A');
    adapter.push_back('B');
    adapter.push_back('C');
    EXPECT_EQ(adapter.view( ), "ABC");
    EXPECT_EQ(len, 3);

    adapter.pop_back( );
    EXPECT_EQ(adapter.view( ), "AB");
    EXPECT_EQ(len, 2);
}

TEST (LenStringAdapterTest, ComparisonOperators) {
    using namespace std::literals;

    char                             buffer[20] = "Hello";
    uint8_t                          len        = 5;
    wbr::len_string_adapter<uint8_t> adapter(buffer, sizeof(buffer), len);

    EXPECT_TRUE(adapter == "Hello"sv);
    EXPECT_FALSE(adapter == "World"sv);
    EXPECT_EQ(adapter <=> "Hello"sv, 0);
    EXPECT_LT(adapter <=> "World"sv, 0);
    EXPECT_GT(adapter <=> "ABC"sv, 0);
}

TEST (LenStringAdapterTest, StringOperations) {
    using namespace std::literals;

    char                             buffer[30] = "Hello, World!";
    uint8_t                          len        = 13;
    wbr::len_string_adapter<uint8_t> adapter(buffer, sizeof(buffer), len);

    EXPECT_TRUE(adapter.starts_with("Hello"));
    EXPECT_TRUE(adapter.ends_with("World!"));
    EXPECT_TRUE(adapter.contains("World"));
    EXPECT_EQ(adapter.find("World"), 7);
    EXPECT_EQ(adapter.substr(0, 5), "Hello");
}

TEST (LenStringAdapterTest, CStrWithSpace) {
    char                             buffer[20] = "Hello";
    uint8_t                          len        = 5;
    wbr::len_string_adapter<uint8_t> adapter(buffer, sizeof(buffer), len);

    const char* cstr = adapter.c_str( );
    EXPECT_NE(cstr, nullptr);
    EXPECT_STREQ(cstr, "Hello");
    EXPECT_EQ(buffer[5], '\0');  // Null terminator added
}

TEST (LenStringAdapterTest, CStrNoSpace) {
    char                             buffer[5] {'H', 'e', 'l', 'l', 'o'};
    uint8_t                          len = 5;
    wbr::len_string_adapter<uint8_t> adapter(buffer, sizeof(buffer), len);

    // Buffer is full, no space for null terminator
    const char* cstr = adapter.c_str( );
    EXPECT_EQ(cstr, nullptr);  // NoCheck strategy returns nullptr
}

TEST (LenStringAdapterTest, BoundCheckException) {
    char                                                            buffer[10];
    uint8_t                                                         len = 0;
    wbr::len_string_adapter<uint8_t, BoundCheckStrategy::Exception> adapter(buffer, sizeof(buffer), len);

    adapter.assign("Short");
    EXPECT_THROW(adapter.append("This is too long"), std::overflow_error);

    // Length should not have changed
    EXPECT_EQ(adapter.view( ), "Short");
    EXPECT_EQ(len, 5);
}

TEST (LenStringAdapterTest, BoundCheckLimitToBound) {
    char                                                               buffer[10];
    uint8_t                                                            len = 0;
    wbr::len_string_adapter<uint8_t, BoundCheckStrategy::LimitToBound> adapter(buffer, sizeof(buffer), len);

    adapter.assign("This is way too long");
    EXPECT_EQ(adapter.size( ), 10);
    EXPECT_EQ(adapter.view( ), "This is wa");
    EXPECT_EQ(len, 10);
}

TEST (LenStringAdapterTest, OperatorPlusEquals) {
    char                             buffer[50];
    uint8_t                          len = 0;
    wbr::len_string_adapter<uint8_t> adapter(buffer, sizeof(buffer), len);

    adapter += "Hello";
    EXPECT_EQ(adapter.view( ), "Hello");

    adapter += ' ';
    EXPECT_EQ(adapter.view( ), "Hello ");

    adapter += "World";
    EXPECT_EQ(adapter.view( ), "Hello World");
    EXPECT_EQ(len, 11);
}

TEST (LenStringAdapterTest, ProtocolMessageExample) {
    struct ChatMessage {
        uint64_t sender_id;
        uint64_t receiver_id;
        char     message[256];
        uint8_t  len;
    };

    ChatMessage msg { };
    msg.sender_id   = 12345;
    msg.receiver_id = 67890;

    wbr::len_string_adapter<uint8_t> adapter(msg.message, 256, msg.len);
    adapter.assign("Hello from sender ");
    adapter.append(std::to_string(msg.sender_id));

    EXPECT_EQ(msg.len, 23);
    EXPECT_EQ(adapter.view( ), "Hello from sender 12345");
}

TEST (LenStringAdapterTest, Uint16LengthType) {
    char                              buffer[300];
    uint16_t                          len = 0;
    wbr::len_string_adapter<uint16_t> adapter(buffer, sizeof(buffer), len);

    // Fill with 280 characters (more than uint8_t max)
    adapter.assign(280, 'X');
    EXPECT_EQ(len, 280);
    EXPECT_EQ(adapter.size( ), 280);
}

TEST (LenStringAdapterTest, MaxSizeConstrainedByLenType) {
    char                             buffer[300];
    uint8_t                          len = 0;
    wbr::len_string_adapter<uint8_t> adapter(buffer, sizeof(buffer), len);

    // max_size should be constrained by uint8_t max (255)
    EXPECT_EQ(adapter.max_size( ), 255);
    EXPECT_EQ(adapter.capacity( ), 255);
}

TEST (LenStringAdapterTest, NoNullTerminationByDefault) {
    char                             buffer[20];
    uint8_t                          len = 0;
    wbr::len_string_adapter<uint8_t> adapter(buffer, sizeof(buffer), len);

    adapter.assign("Hello");
    // Check that we only wrote 5 bytes, not 6 (no automatic null termination)
    EXPECT_EQ(len, 5);

    // Data should still be accessible
    EXPECT_EQ(adapter.view( ), "Hello");
    EXPECT_EQ(std::memcmp(adapter.data( ), "Hello", 5), 0);
}

TEST (LenStringAdapterTest, ViewVsCStr) {
    char                             buffer[10] = "Hello";
    uint8_t                          len        = 5;
    wbr::len_string_adapter<uint8_t> adapter(buffer, sizeof(buffer), len);

    // view() always works
    auto view = adapter.view( );
    EXPECT_EQ(view, "Hello");
    EXPECT_EQ(view.size( ), 5);

    // c_str() adds null termination if space available
    const char* cstr = adapter.c_str( );
    EXPECT_NE(cstr, nullptr);
    EXPECT_STREQ(cstr, "Hello");
}

TEST (LenStringAdapterTest, FullBuffer) {
    char                                                               buffer[5];
    uint8_t                                                            len = 0;
    wbr::len_string_adapter<uint8_t, BoundCheckStrategy::LimitToBound> adapter(buffer, sizeof(buffer), len);

    adapter.assign("12345");
    EXPECT_EQ(adapter.size( ), 5);
    EXPECT_EQ(adapter.view( ), "12345");

    // Try to append more - should be limited
    adapter.append("678");
    EXPECT_EQ(adapter.size( ), 5);  // Still 5
    EXPECT_EQ(adapter.view( ), "12345");
}

TEST (LenStringAdapterTest, ModifyingExternalLength) {
    char                             buffer[20] = "Hello";
    uint8_t                          len        = 5;
    wbr::len_string_adapter<uint8_t> adapter(buffer, sizeof(buffer), len);

    EXPECT_EQ(adapter.size( ), 5);

    // Manually change external length
    len = 3;
    EXPECT_EQ(adapter.size( ), 3);
    EXPECT_EQ(adapter.view( ), "Hel");

    // Operations update the external length
    adapter.append("lo");
    EXPECT_EQ(len, 5);
    EXPECT_EQ(adapter.view( ), "Hello");
}

#if FMT_SUPPORT
TEST (LenStringAdapterTest, FormatAssign) {
    char    buffer[50] = {};
    uint8_t len        = 0;
    wbr::len_string_adapter<uint8_t> adapter(buffer, sizeof(buffer), len);

    adapter.formatAssign("Hello, {}!", "World");
    EXPECT_EQ(adapter.view( ), "Hello, World!");
    EXPECT_EQ(adapter.size( ), 13);
    EXPECT_EQ(len, 13);

    adapter.formatAssign("Number: {}", 42);
    EXPECT_EQ(adapter.view( ), "Number: 42");
    EXPECT_EQ(adapter.size( ), 10);
    EXPECT_EQ(len, 10);

    adapter.formatAssign("{} + {} = {}", 2, 3, 5);
    EXPECT_EQ(adapter.view( ), "2 + 3 = 5");
    EXPECT_EQ(adapter.size( ), 9);
    EXPECT_EQ(len, 9);
}

TEST (LenStringAdapterTest, FormatAppend) {
    char    buffer[50] = {};
    uint8_t len        = 0;
    wbr::len_string_adapter<uint8_t> adapter(buffer, sizeof(buffer), len);
    adapter.assign("Start");
    EXPECT_EQ(len, 5);

    adapter.formatAppend(" {}", "middle");
    EXPECT_EQ(adapter.view( ), "Start middle");
    EXPECT_EQ(adapter.size( ), 12);
    EXPECT_EQ(len, 12);

    adapter.formatAppend(" {}", "end");
    EXPECT_EQ(adapter.view( ), "Start middle end");
    EXPECT_EQ(adapter.size( ), 16);
    EXPECT_EQ(len, 16);
}

TEST (LenStringAdapterTest, FormatAppendChaining) {
    char     buffer[100] = {};
    uint16_t len         = 0;
    wbr::len_string_adapter<uint16_t> adapter(buffer, sizeof(buffer), len);

    adapter.formatAssign("Value: {}", 1).formatAppend(", {}", 2).formatAppend(", {}", 3);

    EXPECT_EQ(adapter.view( ), "Value: 1, 2, 3");
    EXPECT_EQ(adapter.size( ), 14);
    EXPECT_EQ(len, 14);
}

TEST (LenStringAdapterTest, FormatWithVariousTypes) {
    char    buffer[100] = {};
    uint8_t len         = 0;
    wbr::len_string_adapter<uint8_t> adapter(buffer, sizeof(buffer), len);

    adapter.formatAssign("int: {}, double: {:.2f}, string: {}, char: {}", 42, 3.14159, "test", 'X');
    EXPECT_EQ(adapter.view( ), "int: 42, double: 3.14, string: test, char: X");
    EXPECT_EQ(static_cast<int>(len), 44);  // Actual length of the formatted string
}

TEST (LenStringAdapterTest, FormatWithProtocolStructure) {
    struct Message {
        uint64_t sender_id;
        char     text[64];
        uint8_t  text_len;
    } msg = {12345, {}, 0};

    wbr::len_string_adapter<uint8_t> adapter(msg.text, sizeof(msg.text), msg.text_len);

    adapter.formatAssign("User {}: {}", msg.sender_id, "Hello");
    EXPECT_EQ(adapter.view( ), "User 12345: Hello");
    EXPECT_EQ(static_cast<int>(msg.text_len), 17);

    adapter.formatAppend(" at {}", "10:30");
    EXPECT_EQ(adapter.view( ), "User 12345: Hello at 10:30");
    EXPECT_EQ(static_cast<int>(msg.text_len), 26);  // Actual length after append
}
#endif

}  // namespace wbr
