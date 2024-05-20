#include "static_vector.h"

#include <vector>

#include "benchmark/benchmark.h"

using namespace std::literals;

template<typename T, size_t SZ, typename Alloc = std::allocator<T>>
class preallocated_vector
{
    std::vector<T, Alloc> vec;

public:
    using size_type       = typename decltype(vec)::size_type;
    using value_type      = typename decltype(vec)::value_type;
    using iterator        = typename decltype(vec)::iterator;
    using const_iterator  = typename decltype(vec)::const_iterator;
    using reference       = typename decltype(vec)::reference;
    using const_reference = typename decltype(vec)::const_reference;

    constexpr preallocated_vector( ) { vec.reserve(SZ); }

    constexpr preallocated_vector(size_type count) : vec(count) { vec.reserve(SZ); }

    constexpr preallocated_vector(size_type count, const_reference value) : vec(count, value) { vec.reserve(SZ); }

    constexpr preallocated_vector(std::initializer_list<value_type> init) : vec(init) { vec.reserve(SZ); }

    constexpr explicit preallocated_vector(const preallocated_vector& other) : vec {other.vec} { vec.reserve(SZ); }

    constexpr preallocated_vector(preallocated_vector&& other) noexcept : vec(std::move(other.vec)) { vec.reserve(SZ); }

    constexpr preallocated_vector& operator= (const preallocated_vector& other) { vec = other.vec; }

    constexpr preallocated_vector& operator= (preallocated_vector&& other) noexcept { vec = std::move(other.vec); }

    constexpr preallocated_vector& operator= (std::initializer_list<value_type> ilist) { vec = ilist; }

    template<class... Args>
    constexpr auto assign (Args&&... args)
    {
        return vec.assign(std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr auto at (Args&&... args)
    {
        return vec.at(std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr auto at (Args&&... args) const
    {
        return vec.at(std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr auto operator[] (Args&&... args)
    {
        return vec.operator[] (std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr auto operator[] (Args&&... args) const
    {
        return vec.operator[] (std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr auto front (Args&&... args)
    {
        return vec.front(std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr auto front (Args&&... args) const
    {
        return vec.front(std::forward<Args>(args)...);
    }

    constexpr reference back( );

    constexpr const_reference back( ) const;

    constexpr value_type* data( ) noexcept;

    constexpr const value_type* data( ) const;

    template<class... Args>
    constexpr auto begin (Args&&... args)
    {
        return vec.begin(std::forward<Args>(args)...);
    }

    constexpr const_iterator begin( ) const;

    constexpr const_iterator cbegin( ) const;

    template<class... Args>
    constexpr auto end (Args&&... args)
    {
        return vec.end(std::forward<Args>(args)...);
    }

    constexpr const_iterator end( ) const;

    constexpr const_iterator cend( ) const;

    constexpr auto rbegin( );

    constexpr auto rbegin( ) const;

    constexpr auto crbegin( );

    constexpr auto rend( );

    constexpr auto rend( ) const;

    constexpr auto crend( );

    template<class... Args>
    constexpr auto empty (Args&&... args)
    {
        return vec.empty(std::forward<Args>(args)...);
    }

    constexpr size_type size ( ) const { return vec.size( ); }

    constexpr size_type max_size ( ) const { return SZ; }

    constexpr void reserve ( ) { return; }

    constexpr size_type capacity ( ) const { return vec.capacity( ); }

    constexpr void shrink_to_fit ( ) { vec.shrink_to_fit( ); }

    template<class... Args>
    constexpr auto clear (Args&&... args)
    {
        return vec.clear(std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr auto push_back (Args&&... args)
    {
        return vec.push_back(std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr auto emplace_back (Args&&... args)
    {
        return vec.emplace_back(std::forward<Args>(args)...);
    }

    constexpr void pop_back ( ) { vec.pop_back( ); }

    template<class... Args>
    constexpr auto erase (Args&&... args)
    {
        vec.erase(std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr auto insert (Args&&... args)
    {
        vec.insert(std::forward<Args>(args)...);
    }

    template<class... Args>
    constexpr auto resize (Args&&... args)
    {
        vec.resize(std::forward<Args>(args)...);
    }

    constexpr void swap(preallocated_vector& other) noexcept;
};

constexpr size_t maxSizePower = 15;
constexpr size_t minRange     = 10;
constexpr size_t maxRange     = 10;
constexpr size_t cpuCount     = 2;

using i_stdVec = std::vector<int>;
using s_stdVec = std::vector<std::string>;
using i_wbrVec = wbr::static_vector<int, 1 << maxSizePower>;
using s_wbrVec = wbr::static_vector<std::string, 1 << maxSizePower>;
using i_resVec = preallocated_vector<int, 1 << maxSizePower>;
using s_resVec = preallocated_vector<std::string, 1 << maxSizePower>;

template<typename Vector>
void vector_generate_n (benchmark::State& state)
{
    const auto count = 1 << state.range(0);
    for ( auto _: state ) {
        Vector vec;
        std::generate_n(std::back_inserter(vec), count, [] { return 0x11; });
        assert(vec.size( ) == count);
        assert(vec[0] == 0x11);
        assert(vec[count - 1] == 0x11);
        benchmark::DoNotOptimize(vec);
    }
    state.SetComplexityN(state.range(0));
}

BENCHMARK(vector_generate_n<i_stdVec>)->DenseRange(minRange, maxRange, 2)->Complexity(benchmark::o1);
BENCHMARK(vector_generate_n<i_resVec>)->DenseRange(minRange, maxRange, 2)->Complexity(benchmark::o1);
BENCHMARK(vector_generate_n<i_wbrVec>)->DenseRange(minRange, maxRange, 2)->Complexity(benchmark::o1);

template<typename Vector>
void vector_fill (benchmark::State& state)
{
    const auto count = 1 << state.range(0);
    for ( auto _: state ) {
        state.PauseTiming( );
        Vector vec(count);
        benchmark::DoNotOptimize(vec);
        auto first = vec.begin( );
        auto last  = vec.end( );
        state.ResumeTiming( );
        std::fill(first, last, 0x22);
        assert(vec.size( ) == count);
        assert(vec[0] == 0x22);
        assert(vec[count - 1] == 0x22);
    }
}

BENCHMARK(vector_fill<i_stdVec>)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);
BENCHMARK(vector_fill<i_resVec>)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);
BENCHMARK(vector_fill<i_wbrVec>)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);

template<typename Vector>
void vector_begin_insert (benchmark::State& state)
{
    auto count = 1 << state.range(0);
    for ( auto _: state ) {
        Vector vec;
        benchmark::DoNotOptimize(vec);

        for ( int i = 0; i < count; i++ )
            vec.insert(vec.begin( ), 0x33);
        //        assert(vec.size( ) == count);
        //        assert(vec[0] == 0x33);
        //        assert(vec[count - 1] == 0x33);
    }
}

BENCHMARK(vector_begin_insert<i_stdVec>)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);
BENCHMARK(vector_begin_insert<i_resVec>)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);
BENCHMARK(vector_begin_insert<i_wbrVec>)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);

template<typename Vector>
void vector_end_insert (benchmark::State& state)
{
    auto count = 1 << state.range(0);
    for ( auto _: state ) {
        Vector vec;
        for ( int i = 0; i < count; i++ )
            vec.insert(vec.end( ), 0x44);
        assert(vec.size( ) == count);
        assert(vec[0] == 0x44);
        assert(vec[count - 1] == 0x44);
        benchmark::DoNotOptimize(vec);
    }
}

BENCHMARK(vector_end_insert<i_stdVec>)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);
BENCHMARK(vector_end_insert<i_resVec>)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);
BENCHMARK(vector_end_insert<i_wbrVec>)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);

template<typename Vector>
void vector_push_back (benchmark::State& state)
{
    auto count = 1 << state.range(0);
    for ( auto _: state ) {
        Vector vec;
        benchmark::DoNotOptimize(vec);
        for ( int i = 0; i < count; i++ )
            vec.push_back(0x55);
        assert(vec.size( ) == count);
        assert(vec[0] == 0x55);
        assert(vec[count - 1] == 0x55);
    }
}

BENCHMARK(vector_push_back<i_stdVec>)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);
BENCHMARK(vector_push_back<i_resVec>)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);
BENCHMARK(vector_push_back<i_wbrVec>)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);

template<typename Vector>
void vector_clear (benchmark::State& state)
{
    auto count = 1 << state.range(0);
    for ( auto _: state ) {
        state.PauseTiming( );

        Vector vec(count, [] {
            if constexpr ( std::is_arithmetic_v<typename Vector::value_type> )
                return 0x66;
            if constexpr ( std::is_same_v<typename Vector::value_type, std::string> )
                return "short string"s;
        }( ));
        benchmark::DoNotOptimize(vec);
        state.ResumeTiming( );
        vec.clear( );
        assert(vec.size( ) == 0);
    }
}

BENCHMARK(vector_clear<i_stdVec>)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);
BENCHMARK(vector_clear<i_resVec>)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);
BENCHMARK(vector_clear<i_wbrVec>)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);

BENCHMARK(vector_clear<s_stdVec>)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);
BENCHMARK(vector_clear<s_resVec>)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);
BENCHMARK(vector_clear<s_wbrVec>)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);

template<typename Vector>
void vector_erase_begin (benchmark::State& state)
{
    auto count = 1 << state.range(0);
    for ( auto _: state ) {
        Vector vec(count, 0x77);
        benchmark::DoNotOptimize(vec);
        while ( !vec.empty( ) )
            vec.erase(vec.begin( ));
        assert(vec.size( ) == 0);
    }
}

BENCHMARK(vector_erase_begin<i_stdVec>)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);
BENCHMARK(vector_erase_begin<i_resVec>)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);
BENCHMARK(vector_erase_begin<i_wbrVec>)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);

template<typename Vector>
void vector_erase_end (benchmark::State& state)
{
    auto count = 1 << state.range(0);
    for ( auto _: state ) {
        Vector vec(count, 0x88);
        benchmark::DoNotOptimize(vec);
        while ( !vec.empty( ) )
            vec.erase(vec.end( ) - 1);
        assert(vec.size( ) == 0);
    }
}

BENCHMARK(vector_erase_end<i_stdVec>)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);
BENCHMARK(vector_erase_end<i_resVec>)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);
BENCHMARK(vector_erase_end<i_wbrVec>)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);

template<typename Vector>
void vector_create (benchmark::State& state)
{
    const auto count = 1 << state.range(0);
    for ( auto _: state ) {
        Vector vec(count);
        benchmark::DoNotOptimize(vec);
        assert(vec.size( ) == count);
    }
}

BENCHMARK(vector_create<i_stdVec>)->Range(0, 0)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);
BENCHMARK(vector_create<i_resVec>)->Range(0, 0)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);
BENCHMARK(vector_create<i_wbrVec>)->Range(0, 0)->DenseRange(minRange, maxRange, 2)->Threads(cpuCount);
