#include "static_string.hxx"
using namespace wbr;

void static_string_usage ( ) {
    static_string<25> str("uno dos tres");
    fmt::print("static string: {}", str.view( ));
    return;
}
