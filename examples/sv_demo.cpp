#if USE_FMT
    #include <fmt/format.h>
#else
    #include <format>
    #include <print>
    #define fmt std
#endif

#include <numeric>

#include "static_vector.hxx"

int main ( )
{
    wbr::static_vector<int, 20> vec(10);
    std::iota(vec.begin( ), vec.end( ), 0);

    for ( auto a: vec )
        fmt::print("{} ", a);
    fmt::print("\n");

    std::array<std::string, 10> std_array;
    size_t                      count {0};

    wbr::static_vector_adapter adapter {std_array, count};
    adapter.emplace_back("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
    adapter.emplace_back("Duis a nulla vel mauris sagittis volutpat.");
    adapter.emplace_back("Aliquam eu nulla vel ex fringilla tempus in vel libero.");
    adapter.emplace_back("Suspendisse aliquam quam eu volutpat porta.");
    adapter.emplace_back("Fusce a nulla non turpis volutpat sodales.");
    adapter.emplace_back("In sit amet nibh sed dolor viverra consectetur eu a felis.");

    for ( const auto& str: adapter )
        fmt::println("\t• {}", str);

    return 0;
}
