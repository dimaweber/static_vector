#include "static_string.hxx"
#include "len_string.hxx"
#include "string_manipulations.hxx"

using namespace wbr;

void static_string_usage ( ) {
    fmt::println("\n=== static_string example ===");
    static_string<25> str("uno dos tres");
    fmt::println("static string output with fmt: {}", str);

    for ( const auto c: wbr::str::tokenize(str, " ") )
        fmt::println("token: {}", c);

    // Additional operations
    str += " cuatro";
    fmt::println("after append: {}", str);
    fmt::println("length: {}, capacity: {}", str.length(), str.capacity());

    fmt::println("\n=== static_string_adapter example ===");

    // static_string_adapter - adapter for existing char arrays
    char buffer[50] = {};
    static_string_adapter<> adapter(buffer, sizeof(buffer));
    adapter.assign("Hello from adapter");
    fmt::println("adapter content: {}", adapter);
    fmt::println("buffer as c_str: {}", adapter.c_str());

    // Demonstrate adapting different buffer types
    std::array<char, 30> arr_buffer = {};
    static_string_adapter<> arr_adapter(arr_buffer);
    arr_adapter = "std::array adapter";
    fmt::println("array adapter: {}", arr_adapter);

    fmt::println("\n=== len_string_adapter example ===");

    // len_string_adapter - simulates protocol structures with separate length field
    struct Message {
        uint64_t sender_id;
        char text[64];
        uint8_t text_len;
    } msg = {12345, {}, 0};

    len_string_adapter<uint8_t> len_adapter(msg.text, sizeof(msg.text), msg.text_len);
    len_adapter.assign("Protocol message");

    fmt::println("len_string content: {}", len_adapter);
    fmt::println("message length field: {}", msg.text_len);
    fmt::println("sender_id: {}, text: {}, len: {}",
                 msg.sender_id, len_adapter.view(), msg.text_len);

    return;
}
