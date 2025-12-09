#include "len_string.hxx"
#include "static_string.hxx"
#include "string_manipulations.hxx"
using namespace wbr;

#if FMT_SUPPORT
template<typename... Arg>
void prn (fmt::format_string<Arg...> fmt, Arg&&... args) {
    fmt::println(fmt, std::forward<Arg>(args)...);
}
#elif STD_FORMAT_SUPPORT
    #include <print>

template<typename... Arg>
void prn (std::format_string<Arg...> fmt, Arg&&... args) {
    std::println(fmt, std::forward<Arg>(args)...);
}
#else
template<typename... Arg>
void prn (std::string_view fmt, Arg&&... args) {
    using namespace std;
    cout << fmt;
    ((cout << " {" << args << '}') && ...);
    cout << endl;
}
#endif

void static_string_usage ( ) {
    prn("\n=== static_string example ===");
    static_string<25> str("uno dos tres");
    prn("static string output with fmt: {}", str);

    for ( const auto c: wbr::str::tokenize(str, " ") )
        prn("token: {}", c);

    // Additional operations
    str += " cuatro";
    prn("after append: {}", str);
    prn("length: {}, capacity: {}", str.length( ), str.capacity( ));

    prn("\n=== static_string_adapter example ===");

    // static_string_adapter - adapter for existing char arrays
    char                    buffer[50] = { };
    static_string_adapter<> adapter(buffer, sizeof(buffer));
    adapter.assign("Hello from adapter");
    prn("adapter content: {}", adapter);
    prn("buffer as c_str: {}", adapter.c_str( ));

    // Demonstrate adapting different buffer types
    std::array<char, 30>    arr_buffer = { };
    static_string_adapter<> arr_adapter(arr_buffer);
    arr_adapter = "std::array adapter";
    prn("array adapter: {}", arr_adapter);

    prn("\n=== len_string_adapter example ===");

    // len_string_adapter - simulates protocol structures with separate length field
    struct Message {
        uint64_t sender_id;
        char     text[64];
        uint8_t  text_len;
    } msg = {12345, { }, 0};

    len_string_adapter<uint8_t> len_adapter(msg.text, sizeof(msg.text), msg.text_len);
    len_adapter.assign("Protocol message");

    prn("len_string content: {}", len_adapter);
    prn("message length field: {}", msg.text_len);
    prn("sender_id: {}, text: {}, len: {}", msg.sender_id, len_adapter.view( ), msg.text_len);
#if FMT_SUPPORT || STD_FORMAT_SUPPORT
    len_adapter.formatAssign("Protocol {} message", "http");
    prn("len_string content: {}", len_adapter);

    static_string<54> fstr {"There is a {} in {} {}", "house", "New Orlean", 121};
    prn("fstr content: {}", fstr);
#endif

    return;
}
