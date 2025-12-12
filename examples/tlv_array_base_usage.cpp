
#include <fmt/format.h>
#include <fmt/ranges.h>

#include <array>
#include <cstdint>

#include "static_string.hxx"
#include "tlv_vector.hxx"

void tlv_base_usage ( ) {
    using namespace std::literals;

    fmt::println("\n ---- tlv_vector usage example: --- ");

    std::array<uint8_t, 256> array {1, 1, 10, 2, 2, 11, 12, 0, 0};
    wbr::tlv_vector          vec {array};
    for ( auto iter = vec.cbegin( ); iter != vec.end( ); ++iter ) {
        fmt::println("for loop: type : {} length: {}  value: [{:#02x}]", iter->type( ), iter->length( ), fmt::join(iter->raw_value( ), ", "));
    }
    for ( auto& rec: vec ) {
        fmt::println("range-based loop: type : {} length: {}  value: [{:#02x}]", rec.type( ), rec.length( ), fmt::join(rec.raw_value( ), ", "));
    }
    vec.append(3, "string"sv);
    vec.append(7, static_cast<uint32_t>(0xaabbcc));
    vec.append(12, static_cast<int16_t>(-1204));
    for ( auto& rec: vec ) {
        fmt::println("range-based loop: type : {} length: {}  value: [{:#02x}]", rec.type( ), rec.length( ), fmt::join(rec.raw_value( ), ", "));
    }

    fmt::println("uint at type 7: {:#08x}", vec.at(7)->value<uint32_t>( ));
    fmt::println("int at type 12: {}", vec.at(12)->value<int16_t>( ));
    fmt::println("string at type 3: {}", vec.at(3)->value<std::string>( ));

    return;
}
