#include <fmt/format.h>

#include "static_vector.hxx"

void range_based_for_loop ( ) {
    fmt::println("\n ---- range-based for loop usage example: --- ");

    wbr::static_vector<int, 10> vec {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    // Using range-based for loop
    for ( const auto& elem: vec ) {
        fmt::print("{:4} ", elem);
    }
    fmt::print("\n");

    for ( auto& elem: vec ) {
        elem += 13;
    }

    for ( const auto& elem: vec ) {
        fmt::print("{:4} ", elem);
    }
    fmt::print("\n");
}
