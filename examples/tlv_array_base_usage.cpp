
#include <fmt/format.h>
#include <fmt/ranges.h>

#include <array>
#include <cstdint>

#include "tlv_vector.hxx"

void tlv_base_usage ( ) {
    fmt::println("\n ---- tlv_vector usage example: --- ");

    std::array<uint8_t, 256> array {1, 1, 10, 2, 2, 11, 12, 0, 0};
    wbr::tlv_vector          vec {array};
    for ( auto iter = vec.cbegin( ); iter != vec.end( ); ++iter ) {
        fmt::println("for loop: type : {} length: {}  value: [{:#02x}]", iter->type( ), iter->length( ), fmt::join(iter->raw_value( ), ", "));
    }
    for ( auto& rec: vec ) {
        fmt::println("range-based loop: type : {} length: {}  value: [{:#02x}]", rec.type( ), rec.length( ), fmt::join(rec.raw_value( ), ", "));
    }

    return;
}
