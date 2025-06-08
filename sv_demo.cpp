#include <iostream>
#include <numeric>

#include "static_vector.hxx"

int main ( )
{
    wbr::static_vector<int, 20> vec(10);
    std::iota(vec.begin( ), vec.end( ), 0);

    for ( auto a: vec )
        std::cout << a << " ";
    std::cout << std::endl;

    return 0;
}
