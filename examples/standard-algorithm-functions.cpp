#include <fmt/format.h>

#include <algorithm>

#include "static_vector.hxx"

bool is_even (int x)
{
    return x % 2 == 0;
}

void standard_algorithm_functions ( )
{
    fmt::println("\n ---- usage with standard algorithm functions example: --- ");

    wbr::static_vector<int, 10> vec {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    // Using std::for_each
    std::for_each(vec.begin( ), vec.end( ), [] (int x) { fmt::print("{} ", x); });
    fmt::println("");

    // Using std::find
    const auto it = std::find(vec.begin( ), vec.end( ), 5);
    if ( it != vec.end( ) ) {
        fmt::println("Found: {}", *it);
    }

    // Using std::count_if
    const int even_count = std::count_if(vec.begin( ), vec.end( ), is_even);
    fmt::println("Even numbers count: {}", even_count);
}
