#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <stdexcept>

#if __cplusplus >= 202002L
    #include <compare>
#endif
#if __cpp_lib_format >= 201907L
    #include <format>
#endif

#if !defined(wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS)
    #define wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS 0
#endif

#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
    #include <execution>
constexpr auto exec_policy = std::execution::par;
#endif

#if __cplusplus >= 202002L
    #define CXX20_CONSTEXPR constexpr
#else
    #define CXX20_CONSTEXPR
#endif
/** @file
 *
 *  utility class
 *
 */
namespace wbr
{
namespace
{
/**
 * @brief std miss uninitialized variant of @c move_backward function, so define our.
 *
 * @tparam InputIt
 * @tparam NoThrowForwardIt
 * @param first
 * @param last
 * @param d_first
 * @return
 */
template<class InputIt, class NoThrowForwardIt>
NoThrowForwardIt uninitialized_move_backward (InputIt first, InputIt last, NoThrowForwardIt d_first)
{
    if ( d_first == last )
        return first;
#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
    auto iter = std::uninitialized_move(exec_policy, std::reverse_iterator(last), std::reverse_iterator(first), std::reverse_iterator(d_first));
#else
    auto iter = std::uninitialized_move(std::reverse_iterator(last), std::reverse_iterator(first), std::reverse_iterator(d_first));
#endif
    return iter.base( );
}

}  // namespace

enum class BoundCheckStrategy {
    NoCheck,
    UB = NoCheck,
    Assert,
    Exception,
    LimitToBound,
};

template<BoundCheckStrategy bc_strategy>
constexpr void count_fit_capacity_check (size_t count, size_t capacity)
{
    using enum BoundCheckStrategy;

    if constexpr ( bc_strategy == Assert )
        assert(count < capacity);
    if constexpr ( bc_strategy == Exception )
        if ( count >= capacity )
            throw std::out_of_range("count exceeds capacity");
}

constexpr const char* to_string (BoundCheckStrategy e, const char* defValue = nullptr) noexcept
{
    switch ( e ) {
        using enum BoundCheckStrategy;
        case NoCheck:      return "no check (undefined behavior)";
        case Assert:       return "assert";
        case Exception:    return "exception";
        case LimitToBound: return "bound-limit";
    }
    return defValue;
}

template<typename T>
concept IsVector = requires(T vec) {
                       vec.begin( );
                       vec.end( );
                       vec.push_back( );
                       vec.at(size_t { });
                   };

/**
 * @class static_vector_adapter
 * @brief Adapter for {@code std::array} and c-array that provides {@code std::vector}-like interface
 *
 * The static_vector_adapter class is a lightweight wrapper around an {@code std::array} or c-array that provides
 * a subset of functionality similar to {@code std::vector}, but without heap allocation.
 *
 * This adapter is useful when you want to use {@code std::vector}-like operations on a fixed-size array
 * that's allocated on the stack rather than the heap. It maintains size tracking and provides
 * basic container operations.
 *
 * @tparam T The type of elements stored in the array
 * @tparam SZ The fixed size of the array
 */
template<typename T, BoundCheckStrategy bc_strategy = BoundCheckStrategy::UB>
class static_vector_adapter
{
public:
    using value_type      = T;
    using iterator        = value_type*;
    using const_iterator  = const value_type*;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using size_type       = std::size_t;

    /**
     * @brief Constructs a static_vector_adapter with an existing std::array and a counter.
     *
     * This constructor initializes the adapter with a reference to an std::array and a counter
     * that tracks the number of elements currently in use. Various bound check strategies can be
     * applied based on the template parameter bc_strategy:
     * - Exception: Throws an std::out_of_range exception if the counter is out of range.
     * - Assert: Uses an assertion to ensure the counter is within bounds (debug builds only).
     * - LimitToBound: Limits the element count to a valid bound if it's out of range.
     * - UB: Don't perform any checks. Bound violation will lead to undefined behavior
     *
     * @tparam SZ Size of the std::array
     * @param array The std::array to wrap
     * @param counter Reference to the count of elements currently in use (size)
     */
    template<size_type SZ>
    constexpr static_vector_adapter(std::array<T, SZ>& array, std::size_t& counter) noexcept : elements_count_ {counter}, max_elements_count_ {array.size( )}, elements_ {array.data( )}
    {
        if constexpr ( bc_strategy == BoundCheckStrategy::Exception )
            if ( counter > array.size( ) )
                throw std::out_of_range("counter is ouf of range");
        if constexpr ( bc_strategy == BoundCheckStrategy::Assert )
            assert(counter <= array.size( ));
        if constexpr ( bc_strategy == BoundCheckStrategy::LimitToBound )
            elements_count_ = std::min(counter, array.size( ) - 1);
    }

    /**
     * @brief Constructs a static_vector_adapter with an existing C-style array and a counter.
     *
     * This constructor initializes the adapter with a reference to a C-style array and a counter
     * that tracks the number of elements currently in use. Various bound check strategies can be
     * applied based on the template parameter bc_strategy:
     * - Exception: Throws an std::out_of_range exception if the counter is out of range.
     * - Assert: Uses an assertion to ensure the counter is within bounds (debug builds only).
     * - LimitToBound: Limits the element count to a valid bound if it's out of range.
     * - UB: Don't perform any checks. Bound violation will lead to undefined behavior
     *
     * @tparam N Size of the C-style array
     * @param array The C-style array to wrap
     * @param counter Reference to the count of elements currently in use (size)
     */
    template<size_t N>
    constexpr static_vector_adapter(T (&array)[N], std::size_t& counter) noexcept : elements_count_ {counter}, max_elements_count_ {N}, elements_ {array}
    {
        if constexpr ( bc_strategy == BoundCheckStrategy::Exception )
            if ( counter > N )
                throw std::out_of_range("counter is ouf of range");
        if constexpr ( bc_strategy == BoundCheckStrategy::Assert )
            assert(counter <= N);
        if constexpr ( bc_strategy == BoundCheckStrategy::LimitToBound )
            elements_count_ = std::min(counter, N - 1);
    }

    /**
     * @brief Constructs a static_vector_adapter with a raw pointer array and a counter.
     *
     * This constructor initializes the adapter with a reference to a raw pointer array and a counter
     * that tracks the number of elements currently in use. Various bound check strategies can be
     * applied based on the template parameter bc_strategy:
     * - Exception: Throws an std::out_of_range exception if the counter is out of range.
     * - Assert: Uses an assertion to ensure the counter is within bounds (debug builds only).
     * - LimitToBound: Limits the element count to a valid bound if it's out of range.
     * - UB: Don't perform any checks. Bound violation will lead to undefined behavior
     *
     * @param array Pointer to the raw array
     * @param array_max_size Maximum size of the array
     * @param counter Reference to the count of elements currently in use (size)
     */
    constexpr static_vector_adapter (T* array, std::size_t array_max_size, std::size_t& counter) noexcept : elements_count_ {counter}, max_elements_count_ {array_max_size}, elements_ {array}
    {
        if constexpr ( bc_strategy == BoundCheckStrategy::Exception )
            if ( counter > array_max_size )
                throw std::out_of_range("counter is ouf of range");
        if constexpr ( bc_strategy == BoundCheckStrategy::Assert )
            assert(counter <= array_max_size);
        if constexpr ( bc_strategy == BoundCheckStrategy::LimitToBound )
            elements_count_ = std::min(counter, array_max_size - 1);
    }

    /**
     * @brief Default destructor
     *
     * The destructor does not perform any cleanup as it doesn't own the underlying array.
     */
    constexpr ~static_vector_adapter( ) = default;

    // Delete copy/move constructors and assignment operators to ensure the adapter
    // cannot be copied or moved (as it manages a reference to an external array)
    static_vector_adapter(const static_vector_adapter&) = delete;
    static_vector_adapter(static_vector_adapter&&)      = delete;

    static_vector_adapter& operator= (const static_vector_adapter&) = delete;
    static_vector_adapter& operator= (static_vector_adapter&&)      = delete;

    /**
     * @brief Assigns a given value to first `count` elements in the adapter
     *
     * This function clears any existing elements and then assigns `count` copies of `value`.
     * It supports different bound check strategies based on the template parameter `bc_strategy`:
     * - @c Assert: Uses an assertion to ensure count does not exceed capacity (debug builds only).
     * - @c Exception: Throws an `std::out_of_range` exception if `count` exceeds capacity.
     * - @c LimitToBound: Limits `count` to a valid bound if it's out of range.
     * - @c UB: Don't perform any checks. Bound violation will lead to undefined behavior
     *
     * @param count The number of elements to assign
     * @param value The value to assign to each element
     *
     * @throws std::out_of_range if `count` > `capacity()` and `bc_strategy` is `Exception`
     *
     * @code{.cpp}
     * #include <iostream>
     * #include "static_vector_adapter.h"
     *
     * int main() {
     *     std::array<int, 5> data_array = {0, 1, 2, 3, 4};
     *     std::size_t element_count = 3; // Only using first 3 elements
     *
     *     static_vector_adapter adapter(data_array, element_count);
     *
     *     // Assign a single value to all elements
     *     adapter.assign(3, 99);
     *
     *     // Print the array contents
     *     for (int i = 0; i < data_array.size(); ++i) {
     *         std::cout << "data_array[" << i << "] = " << data_array[i] << "\n";
     *     }
     *
     *     return 0;
     * }
     * @endcode
     */
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr void assign (size_type count, const T& value)
    {
        using enum BoundCheckStrategy;

        if constexpr ( custom_bc_strategy == Assert )
            assert(count <= capacity( ));
        if constexpr ( custom_bc_strategy == Exception )
            if ( count > capacity( ) )
                throw std::out_of_range("count exceeds capacity");
        if constexpr ( custom_bc_strategy == LimitToBound )
            count = std::min(count, capacity( ));

        clear( );
        std::uninitialized_fill_n(begin( ), count, value);
        elements_count_ = count;
    }

    /**
     * @brief Assigns values from an input iterator range
     *
     * This function clears any existing elements and then copies elements from the range [first, last).
     *
     * @tparam InputIt Type of input iterators
     * @param first Iterator to the beginning of the range
     * @param last Iterator to the end of the range
     *
     * @throws std::out_of_range if distance(first, last) > SZ
     *
     * @code{.cpp}
     * #include <iostream>
     * #include "static_vector_adapter.h"
     *
     * int main() {
     *     std::array<int, 5> data_array = {0, 1, 2, 3, 4};
     *     std::size_t element_count = 3; // Only using first 3 elements
     *
     *     static_vector_adapter adapter(data_array, element_count);
     *
     *     // Assign values from a different array
     *     int other_data[] = {10, 20, 30};
     *     adapter.assign(other_data, other_data + 3);
     *
     *     // Print the array contents
     *     for (int i = 0; i < data_array.size(); ++i) {
     *         std::cout << "data_array[" << i << "] = " << data_array[i] << "\n";
     *     }
     *
     *     return 0;
     * }
     * @endcode
     */
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr void assign (std::input_iterator auto first, std::input_iterator auto last)
    {
        using enum BoundCheckStrategy;

        auto              n   = std::distance(first, last);
        const decltype(n) cap = capacity( );

        if constexpr ( custom_bc_strategy == Assert ) {
            assert(n <= cap);
            assert(n > 0);
        }
        if constexpr ( custom_bc_strategy == Exception ) {
            if ( n > cap )
                throw std::out_of_range {std::format("count {} exceeds capacity {}", n, cap)};
            if ( n < 0 )
                throw std::runtime_error("start iterator is greater then finish");
        }
        if constexpr ( custom_bc_strategy == LimitToBound ) {
            n = std::min(n, cap);
            n = std::max(n, static_cast<decltype(n)>(0));
        }

        clear( );
        std::uninitialized_copy_n(first, n, begin( ));
        elements_count_ = n;
    }

    /**
     * @brief Assigns values from an initializer list
     *
     * This function clears any existing elements and then copies elements from the initializer list.
     *
     * @param ilist Initializer list to copy elements from
     *
     * @throws std::out_of_range if ilist.size() > SZ
     *
     * @code{.cpp}
     * #include <iostream>
     * #include "static_vector_adapter.h"
     *
     * int main() {
     *     std::array<int, 5> data_array = {0, 1, 2, 3, 4};
     *     std::size_t element_count = 3; // Only using first 3 elements
     *
     *     static_vector_adapter adapter(data_array, element_count);
     *
     *     // Assign values from an initializer list
     *     adapter.assign({100, 200, 300});
     *
     *     // Print the array contents
     *     for (int i = 0; i < data_array.size(); ++i) {
     *         std::cout << "data_array[" << i << "] = " << data_array[i] << "\n";
     *     }
     *
     *     return 0;
     * }
     * @endcode
     */
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr void assign (std::initializer_list<T> ilist)
    {
        auto first = ilist.begin( );
        auto last  = ilist.end( );

        using enum BoundCheckStrategy;
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Exception )
            if ( ilist.size( ) > capacity( ) )
                throw std::overflow_error("initializer list is longer then container capacity");
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Assert )
            assert(ilist.size( ) <= capacity( ));
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::LimitToBound )
            last = std::min(last, first + capacity( ));

        return assign(first, last);
    }

    /**
     * @brief Access the element at specified position with bounds checking.
     *
     * This method provides access to an element at a specific position in the container,
     * performing a bounds check and throwing an exception if the index is out of range.
     *
     * @param pos The position of the element to access.
     * @return Reference to the element at the specified position.
     * @throws std::out_of_range If `pos` is not within the valid range [0, size()).
     *
     * Example usage:
     * @code
     * std::array<int, 5> data_array = {0, 1, 2, 3, 4};
     * std::size_t element_count = 5;
     * static_vector_adapter vec {data_array, element_count};
     *
     * try {
     *     int val& = vec.at(2); // Access the element at position 2 (value is 3)
     *     vec.at(3) = 40;
     * } catch (const std::out_of_range& e) {
     *     std::cerr << "Out of range error: " << e.what() << '\n';
     * }
     * @endcode
     */
    [[nodiscard]] constexpr const_reference at (size_type pos) const
    {
        if ( pos >= size( ) )
#if __cpp_lib_format >= 201907L
            throw std::out_of_range {std::format("static_vector::{}: pos (which is {}) >= this->size() (which is {})", __func__, pos, size( ))};
#else
            throw std::out_of_range {"static_vector::range_check: pos >= this->size()"};
#endif
        return operator[] (pos);
    }

    /**
     * @brief Access the element at specified position with bounds checking (const version).
     *
     * This method provides access to an element at a specific position in the container,
     * performing a bounds check and throwing an exception if the index is out of range.
     *
     * @param pos The position of the element to access.
     * @return Const reference to the element at the specified position.
     * @throws std::out_of_range If `pos` is not within the valid range [0, size()).
     *
     * Example usage:
     * @code
     * std::array<int, 5> data_array = {0, 1, 2, 3, 4, 5};
     * std::size_t element_count = 5;
     * const static_vector_adapter vec {data_array, element_count};
     *
     * try {
     *     const int& val = vec.at(2); // Access the element at position 2 (value is 3)
     * } catch (const std::out_of_range& e) {
     *     std::cerr << "Out of range error: " << e.what() << '\n';
     * }
     * @endcode
     */
    [[nodiscard]] constexpr auto at (size_type pos) -> reference
    {
        if ( pos >= size( ) )
#if __cpp_lib_format >= 201907L
            throw std::out_of_range {std::format("static_vector::{}: pos (which is {}) >= this->size() (which is {})", __func__, pos, size( ))};
#else
            throw std::out_of_range {"static_vector::range_check: pos >= this->size()"};
#endif
        return operator[] (pos);
    }

    /**
     * @brief Access the element at specified position.
     *
     * This method provides access to an element at a specific position in the container,
     * without performing bounds checking. The caller is responsible for ensuring that
     * the index is valid (i.e., within the range [0, size())).
     *
     * @param pos The position of the element to access.
     * @return Reference to the element at the specified position.
     * @note No bounds checking is performed. Accessing out-of-bounds indices results in undefined behavior.
     *
     * Example usage:
     * @code
     * std::array<int, 5> data_array = {1, 2, 3, 4, 5};
     * std::size_t element_count = 5;
     * static_vector_adapter vec  {data_array, element_count};
     *
     * int& val = vec[2]; // Access the element at position 2 (value is 3)
     * vec[3] = 40;
     * @endcode
     */
    [[nodiscard]] constexpr reference operator[] (size_t pos) noexcept(bc_strategy != BoundCheckStrategy::Exception)
    {
        if constexpr ( bc_strategy == BoundCheckStrategy::Exception )
            if ( pos >= size( ) )
                throw std::out_of_range("pos is out of  range");
        if constexpr ( bc_strategy == BoundCheckStrategy::Assert )
            assert(pos < size( ));
        if constexpr ( bc_strategy == BoundCheckStrategy::LimitToBound )
            pos = std::min(pos, size( ) - 1);

        return *(begin( ) + pos);
    }

    /**
     * @brief Access the element at specified position (const version).
     *
     * This method provides access to an element at a specific position in the container,
     * without performing bounds checking. The caller is responsible for ensuring that
     * the index is valid (i.e., within the range [0, size())).
     *
     * @param pos The position of the element to access.
     * @return Const reference to the element at the specified position.
     * @note No bounds checking is performed. Accessing out-of-bounds indices results in undefined behavior.
     *
     * Example usage:
     * @code
     * const static_vector_adapter<int, 5> vec = {1, 2, 3, 4, 5};
     * int val = vec[2]; // Access the element at position 2 (value is 3)
     * @endcode
     */
    [[nodiscard]] constexpr const_reference operator[] (size_t pos) const noexcept(bc_strategy != BoundCheckStrategy::Exception)
    {
        if constexpr ( bc_strategy == BoundCheckStrategy::Exception )
            if ( pos >= size( ) )
                throw std::out_of_range("pos is out of  range");
        if constexpr ( bc_strategy == BoundCheckStrategy::Assert )
            assert(pos < size( ));
        if constexpr ( bc_strategy == BoundCheckStrategy::LimitToBound )
            pos = std::min(pos, size( ) - 1);

        return *(begin( ) + pos);
    }

    /**
     * @brief Returns a reference to the first element in the container.
     *
     * This function returns a reference to the first element of the array. If the container is empty,
     * it calls `not_empty_container_check()` which may throw an exception or assert.
     *
     * @return Reference to the first element.
     *
     * @throws {@code std::out_of_range} if the container is empty (depending on implementation of {@code not_empty_container_check}).
     *
     * Example usage:
     * @code
     * std::array<int, 5> data = {1, 2, 3, 4, 5};
     * size_t count = 5;
     * static_vector_adapter<int, 5> vec(data, count);
     *
     * int firstElement = vec.front(); // Accesses the first element
     * std::cout << "First element: " << firstElement << std::endl; // Outputs: First element: 1
     * @endcode
     */
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    [[nodiscard]] constexpr reference front ( ) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception)
    {
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Exception )
            if ( empty( ) )
                throw std::out_of_range("empty container");
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Assert )
            assert(!empty( ));

        return *begin( );
    }

    /**
     * @brief Returns a const reference to the first element in the container.
     *
     * This function returns a const reference to the first element of the array. If the container is empty,
     * it calls `not_empty_container_check()` which may throw an exception or assert.
     *
     * @return Const reference to the first element.
     *
     * @throws {@code std::out_of_range} if the container is empty (depending on implementation of `not_empty_container_check`).
     */
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    [[nodiscard]] constexpr const_reference front ( ) const noexcept(custom_bc_strategy != BoundCheckStrategy::Exception)
    {
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Exception )
            if ( empty( ) )
                throw std::out_of_range("empty container");
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Assert )
            assert(!empty( ));

        return *begin( );
    }

    /**
     * @brief Returns a reference to the last element in the container.
     *
     * This function returns a reference to the last element of the array. If the container is empty,
     * it calls `not_empty_container_check()` which may throw an exception or assert.
     *
     * @return Reference to the last element.
     *
     * @throws {@code std::out_of_range} if the container is empty (depending on implementation of `not_empty_container_check`).
     *
     * Example usage:
     * @code
     * std::array<int, 5> data = {1, 2, 3, 4, 5};
     * size_t count = 5;
     * static_vector_adapter<int, 5> vec(data, count);
     *
     * int lastElement = vec.back(); // Accesses the last element
     * std::cout << "Last element: " << lastElement << std::endl; // Outputs: Last element: 5
     * @endcode
     */
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    [[nodiscard]] constexpr reference back ( ) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception)
    {
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Exception )
            if ( empty( ) )
                throw std::out_of_range("empty container");
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Assert )
            assert(!empty( ));

        return *std::prev(end( ));
    }

    /**
     * @brief Returns a const reference to the last element in the container.
     *
     * This function returns a const reference to the last element of the array. If the container is empty,
     * it calls `not_empty_container_check()` which may throw an exception or assert.
     *
     * @return Const reference to the last element.
     *
     * @throws {@code std::out_of_range} if the container is empty (depending on implementation of `not_empty_container_check`).
     */
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    [[nodiscard]] constexpr const_reference back ( ) const noexcept(custom_bc_strategy != BoundCheckStrategy::Exception)
    {
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Exception )
            if ( empty( ) )
                throw std::out_of_range("empty container");
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Assert )
            assert(!empty( ));

        return *std::prev(end( ));
    }

    /**
     * @brief Clears all elements in the adapter.
     *
     * This function destroys all elements and resets the element count to 0. The capacity of
     * the container remains unchanged.
     *
     * @note If `T` is not trivially destructible, it will call the destructor for each element.
     *
     * @throws Do not throw.
     *
     * Example usage:
     * @code
     * std::array<int,5> data_array = {1, 2, 3, 4, 5};
     * std::size_t element_count = 5;
     * static_vector_adapter vec {data_array, element_count};
     *
     * std::cout << "Size before clear: " << vec.size() << std::endl; // Outputs: Size before clear: 5
     *
     * vec.clear(); // Clear all elements
     *
     * std::cout << "Size after clear: " << vec.size() << std::endl; // Outputs: Size after clear: 0
     * @endcode
     */
    constexpr void clear ( ) noexcept
    {
        if constexpr ( !std::is_trivially_destructible_v<T> )
            std::destroy_n(begin( ), elements_count_);
        elements_count_ = 0;
    }

    constexpr pointer data ( ) noexcept { return elements_; }

    constexpr const_pointer data ( ) const noexcept { return elements_; }

    /**
     * @brief Gets an iterator to the beginning of the container
     *
     * @throws Do not throw.
     * @return Iterator pointing to the first element
     */
    [[nodiscard]] constexpr iterator begin ( ) noexcept { return data( ); }

    [[nodiscard]] constexpr const_iterator cbegin ( ) noexcept { return data( ); }

    [[nodiscard]] constexpr const_iterator begin ( ) const noexcept { return cbegin( ); }

    /**
     * @brief Returns an iterator to the end of the container.
     *
     * This function returns an iterator pointing to one past the last element in the array. This is useful for
     * range-based loops and other algorithms that require an end iterator.
     *
     * @throws Do not throw.
     * @return Iterator to the end of the container.
     *
     * Example usage:
     * @code
     * std::array<int, 5> data = {1, 2, 3, 4, 5};
     * std::size_t count = 5;
     * static_vector_adapter vec(data, count);
     *
     * for (auto it = vec.begin(); it != vec.end(); ++it) {
     *     std::cout << "Element: " << *it << std::endl; // Prints all elements
     * }
     * @endcode
     */
    [[nodiscard]] constexpr iterator end ( ) noexcept { return begin( ) + size( ); }

    [[nodiscard]] constexpr const_iterator cend ( ) noexcept { return cbegin( ) + size( ); }

    [[nodiscard]] constexpr const_iterator end ( ) const noexcept { return cend( ); }

    [[nodiscard]] constexpr auto rbegin ( ) noexcept { return std::make_reverse_iterator(end( )); }

    [[nodiscard]] constexpr auto crbegin ( ) noexcept { return std::make_reverse_iterator(cend( )); }

    [[nodiscard]] constexpr auto rbegin ( ) const noexcept { return crbegin( ); }

    [[nodiscard]] constexpr auto rend ( ) noexcept { return std::make_reverse_iterator(begin( )); }

    [[nodiscard]] constexpr auto crend ( ) noexcept { return std::make_reverse_iterator(cbegin( )); }

    [[nodiscard]] constexpr auto rend ( ) const noexcept { return crend( ); }

    /**
     * @brief Returns the maximum number of elements that can be stored in the container.
     *
     * This function returns the maximum size of the container, which is determined by the fixed size of the underlying {@code std::array}.
     *
     * @throws Do not throw.
     * @return Maximum size of the container.
     *
     * Example usage:
     * @code
     * std::array<int, 5> data = {1, 2, 3, 4, 5};
     * size_t count = 5;
     * static_vector_adapter<int, 5> vec(data, count);
     *
     * std::cout << "Max size: " << vec.max_size() << std::endl; // Outputs: Max size: 5
     * @endcode
     */
    [[nodiscard]] constexpr size_type max_size ( ) const noexcept { return max_elements_count_; }

    [[nodiscard]] constexpr size_type capacity ( ) const noexcept { return max_size( ); }

    /**
     * @brief Returns the number of elements currently stored in the container.
     *
     * This function returns the current size (number of elements) that are actively used in the array. Note that
     * this may be less than or equal to the maximum size.
     *
     * @throws Do not throw.
     * @return Number of elements currently in use.
     *
     * Example usage:
     * @code
     * {@code}std::array<int, 5>{@code} data = {1, 2, 3, 4, 5};
     * size_t count = 5;
     * static_vector_adapter<int, 5> vec(data, count);
     *
     * std::cout << "Current size: " << vec.size() << std::endl; // Outputs: Current size: 5
     * @endcode
     */
    [[nodiscard]] constexpr size_type size ( ) const noexcept { return elements_count_; }

    /**
     * @brief Checks if the container is empty.
     *
     * This function returns true if the number of elements currently stored in the container is 0, otherwise it returns false.
     *
     * @throws Do not throw.
     * @return True if the container has no elements, false otherwise.
     *
     * Example usage:
     * @code
     * {@code}std::array<int, 5>{@code} data = {1, 2, 3, 4, 5};
     * size_t count = 0; // Simulate an empty container
     * static_vector_adapter<int, 5> vec(data, count);
     *
     * if (vec.empty()) {
     *     std::cout << "The container is empty." << std::endl;
     * } else {
     *     std::cout << "The container is not empty." << std::endl; // Outputs: The container is empty.
     * }
     * @endcode
     */
    [[nodiscard]] constexpr bool empty ( ) const noexcept { return size( ) == 0; }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr void pop_back ( ) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception)
    {
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Exception )
            if ( empty( ) )
                throw std::out_of_range("empty container");
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Assert )
            assert(!empty( ));
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::LimitToBound )
            if ( empty( ) )
                return;

        if constexpr ( !std::is_trivially_destructible_v<value_type> )
            std::destroy_at(begin( ) + elements_count_ - 1);
        --elements_count_;
    }

    template<class... Args>
    constexpr reference emplace_back (Args&&... args)
    {
        if constexpr ( bc_strategy == BoundCheckStrategy::Exception )
            if ( size( ) == capacity( ) )
                throw std::overflow_error("attempt to insert to container led to overflow");
        if constexpr ( bc_strategy == BoundCheckStrategy::Assert )
            assert(size( ) < capacity( ));
        if constexpr ( bc_strategy == BoundCheckStrategy::LimitToBound )
            if ( size( ) == capacity( ) )
                return back( );

#if __cplusplus >= 202002L
        std::construct_at<value_type>(begin( ) + elements_count_, std::forward<Args>(args)...);
#else
        ::new (begin( ) + elementsCount) value_type(std::forward<Args>(args)...);
#endif
        ++elements_count_;
        return back( );
    }

    constexpr void push_back (value_type&& value)
    {
        if constexpr ( bc_strategy == BoundCheckStrategy::Exception )
            if ( size( ) == capacity( ) )
                throw std::overflow_error("attempt to insert to container led to overflow");
        if constexpr ( bc_strategy == BoundCheckStrategy::Assert )
            assert(size( ) < capacity( ));
        if constexpr ( bc_strategy == BoundCheckStrategy::LimitToBound )
            if ( size( ) == capacity( ) )
                return;

        emplace_back(std::move(value));
    }

    constexpr void push_back (const_reference value)
    {
        if constexpr ( bc_strategy == BoundCheckStrategy::Exception )
            if ( size( ) == capacity( ) )
                throw std::overflow_error("attempt to insert to container led to overflow");
        if constexpr ( bc_strategy == BoundCheckStrategy::Assert )
            assert(size( ) < capacity( ));
        if constexpr ( bc_strategy == BoundCheckStrategy::LimitToBound )
            if ( size( ) == capacity( ) )
                return;

#if __cplusplus >= 202002L
        std::construct_at<value_type>(end( ), value);
#else
        ::new (end( )) value_type(value);
#endif
        ++elements_count_;
    }

    constexpr iterator erase (const_iterator pos)
    {
        // valid_iterator_check(pos);

        if ( elements_count_ == 0 )
            return end( );

        const auto offset = std::distance(cbegin( ), pos);

        if constexpr ( !std::is_trivially_destructible_v<value_type> )
            std::destroy_at(begin( ) + offset);
        if ( pos != cend( ) - 1 )
            shift_elements_left(offset, 1);
        --elements_count_;
        return begin( ) + offset;
    }

    constexpr iterator erase (const_iterator first, const_iterator last)
    {
        // valid_iterator_check(first);
        // valid_iterator_check(last);
        // valid_range_check(first, last);

        const auto offset = std::distance(cbegin( ), first);
        const auto length = std::distance(first, last);

        if constexpr ( !std::is_trivially_destructible_v<value_type> )
            std::destroy_n(begin( ) + offset, length);
        if ( last != cend( ) )
            shift_elements_left(offset, length);

        elements_count_ -= length;
        return begin( ) + offset;
    }

    /**
     * @brief Inserts a single element at the specified position.
     *
     * This function inserts a copy of the given value before the element at the specified
     * position, effectively increasing the container size by one. The current element at pos
     * and all elements beyond it are shifted one position to the right.
     *
     * The method supports different bound check strategies:
     * - Exception: Throws an exception if the insertion is out of bounds.
     * - Assert: Uses assertions to ensure that the insertion is within bounds (use in debug mode).
     * - LimitToBound: Limits the position to valid boundaries without throwing exceptions or using assertions.
     * - UB: Don't perform any checks. Bound violation will lead to undefined behavior
     *
     * @tparam custom_bc_strategy Strategy for bound checking (defaults to bc_strategy)
     * @param pos Iterator pointing to the position where the new element will be inserted
     * @param value Value to insert before the specified position
     * @return Iterator pointing to the newly inserted element
     *
     * @throws std::out_of_range If the insertion is out of bounds and Exception strategy is used.
     *
     * @code
     * static_vector_adapter<int, 5> adapter;
     * for (size_t i = 0; i < 3; ++i) {
     *     adapter.push_back(i);
     * }
     * auto it = adapter.insert(adapter.begin() + 1, 99); // Inserts 99 at position 1
     *
     * // Output: [0, 99, 1, 2]
     * @endcode
     */
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr iterator insert (const_iterator pos, const_reference value)
    {
        using enum BoundCheckStrategy;
        if constexpr ( custom_bc_strategy == Exception ) {
            if ( size( ) + 1 > capacity( ) )
                throw std::overflow_error("vector is full, no new element can be inserted");
            if ( pos < begin( ) || pos > end( ) )
                throw std::out_of_range("pos is out of range");
        }
        if constexpr ( custom_bc_strategy == Assert ) {
            assert(size( ) + 1 <= capacity( ));
            assert(pos >= begin( ) && pos <= end( ));
        }
        if constexpr ( custom_bc_strategy == LimitToBound ) {
            if ( size( ) >= capacity( ) )
                return end( );
            if ( pos > end( ) )
                pos = end( );
            if ( pos < begin( ) )
                pos = begin( );
        }

        const auto offset = std::distance(cbegin( ), pos);
        if ( offset != elements_count_ )
            shift_elements_right(offset, 1);

#if __cplusplus >= 202002L
        std::construct_at<value_type>(begin( ) + offset, value);
#else
        ::new (begin( ) + offset) value_type(value);
#endif
        ++elements_count_;
        return begin( ) + offset;
    }

    /**
     * @brief Inserts a value at the specified position.
     *
     * This function inserts a new element into the static vector at the position
     * specified by `pos`. The iterator `pos` must be valid and within the range of
     * the container. If `pos` is not at the end, existing elements are shifted to
     * the right to make space for the new element.
     *
     * @tparam T The type of elements in the static vector.
     * @tparam custom_bc_strategy The bound check strategy to use. Possible values are:
     * - Exception: Throws an exception if bounds are violated
     * - Assert: Uses assertions to check bounds (debug mode only)
     * - LimitToBound: Silently limits the position to valid range without throwing
     * - UB: don't perform any checks. Bound violation will lead to undefined behavior
     * @param pos An iterator pointing to the position where the value should be inserted.
     * @param value The value to insert. This is an rvalue reference, indicating that the value may be
     *              moved from rather than copied.
     * @return An iterator pointing to the newly inserted element.
     *
     * @throw std::overflow_error if the vector is full (size + 1 > capacity) and `custom_bc_strategy` is set to Exception
     * @throw std::out_of_range if pos is out of range and `custom_bc_strategy` is set to Exception
     */
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr iterator insert (const_iterator pos, value_type&& value)
    {
        using enum BoundCheckStrategy;
        if constexpr ( custom_bc_strategy == Exception ) {
            if ( size( ) + 1 > capacity( ) )
                throw std::overflow_error("vector is full, no new element can be inserted");
            if ( pos < begin( ) || pos > end( ) )
                throw std::out_of_range("pos is out of range");
        }
        if constexpr ( custom_bc_strategy == Assert ) {
            assert(size( ) + 1 <= capacity( ));
            assert(pos >= begin( ) && pos <= end( ));
        }
        if constexpr ( custom_bc_strategy == LimitToBound ) {
            if ( size( ) >= capacity( ) )
                return end( );
            if ( pos > end( ) )
                pos = end( );
            if ( pos < begin( ) )
                pos = begin( );
        }

        const auto offset = std::distance(cbegin( ), pos);
        if ( offset != elements_count_ )
            shift_elements_right(offset, 1);

#if __cplusplus >= 202002L
        std::construct_at<value_type>(begin( ) + offset, std::move(value));
#else
        ::new (begin( ) + offset) value_type(std::move(value));
#endif
        ++elements_count_;
        return begin( ) + offset;
    }

    constexpr iterator insert (const_iterator pos, size_type count, const_reference value)
    {
        // count_overflow_check(count);
        // valid_iterator_check(pos);

        const auto offset = std::distance(cbegin( ), pos);

        if ( offset != elements_count_ )
            shift_elements_right(offset, count);
#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
        std::uninitialized_fill_n(exec_policy, begin( ) + offset, count, value);
#else
        std::uninitialized_fill_n(begin( ) + offset, count, value);
#endif

        elements_count_ += count;
        return begin( ) + offset;
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr iterator insert (const_iterator pos, std::input_iterator auto first, std::input_iterator auto last)
    {
        // count_overflow_check(std::distance(first, last));
        // valid_iterator_check(pos);
        // valid_range_check(first, last);

        const auto offset = std::distance(cbegin( ), pos);
        const auto length = std::distance(first, last);

        if ( offset != elements_count_ )
            shift_elements_right(offset, length);
#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
        std::uninitialized_copy(exec_policy, first, last, begin( ) + offset);
#else
        std::uninitialized_copy(first, last, begin( ) + offset);
#endif

        elements_count_ += length;
        return begin( ) + offset;
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr iterator insert (const_iterator pos, std::initializer_list<value_type> ilist)
    {
        // count_overflow_check(ilist.size( ));
        // valid_iterator_check(pos);

        return insert(pos, ilist.begin( ), ilist.end( ));
    }

    template<class... Args>
    constexpr iterator emplace (const_iterator pos, Args&&... args)
    {
        //    valid_iterator_check(pos);

        const auto offset = std::distance(cbegin( ), pos);

        if ( offset != elements_count_ )
            shift_elements_right(offset, 1);
#if __cplusplus >= 202002L
        std::construct_at<value_type>(begin( ) + offset, std::forward<Args>(args)...);
#else
        ::new (begin( ) + offset) value_type(std::forward<Args>(args)...);
#endif
        ++elements_count_;
        return begin( ) + offset;
    }

    constexpr void resize (size_type count, const_reference value)
    {
        if ( count > max_size( ) )
            throw std::length_error {"capacity would exceed max_size()"};
        if ( count > size( ) ) {
            insert(end( ), count - size( ), value);
        } else if ( count < size( ) ) {
            erase(begin( ) + count, end( ));
        }
    }

    constexpr void resize (size_type count) { return resize(count, value_type { }); }

private:
    static constexpr size_t element_size_ = sizeof(value_type);

    size_type&      elements_count_;  ///< Reference to count of currently used elements
    const size_type max_elements_count_;
    const pointer   elements_;

    constexpr void shift_elements_right (size_type offset, size_type count)
    {
        iterator first = data( ) + offset;
        iterator last  = data( ) + elements_count_;
        iterator dlast = data( ) + elements_count_ + count;

        if constexpr ( std::is_trivially_copyable_v<T> )
            std::move_backward(first, last, dlast);
        else
            uninitialized_move_backward(begin( ) + offset, end( ), end( ) + count);
    }

    constexpr void shift_elements_left (size_type offset, size_type count)
    {
        iterator first  = data( ) + offset + count;
        iterator last   = data( ) + elements_count_;
        iterator dfirst = data( ) + offset;

        if constexpr ( std::is_trivially_copyable_v<T> )
            std::move(first, last, dfirst);
        else
#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
            std::uninitialized_move(exec_policy, begin( ) + offset + count, end( ), begin( ) + offset);
#else
            std::uninitialized_move(begin( ) + offset + count, end( ), begin( ) + offset);
#endif
    }
};

/**
 * @brief Creates a static_vector_adapter from an std::array and count.
 *
 * This function creates a static_vector_adapter by initializing it with a reference to
 * an std::array and the number of elements currently in use within that array.
 *
 * @tparam T The type of elements stored in the adapter.
 * @tparam SZ The fixed size of the adapter (number of elements in the underlying std::array).
 * @param arr A reference to the std::array containing the data.
 * @param count A reference to a variable representing the number of elements currently used.
 *
 * Example usage:
 * @code
 * #include <iostream>
 * #include <array>
 *
 * int main() {
 *     std::array<int, 5> myArray = {1, 2, 3, 4, 5};
 *     size_t elementCount = 5;
 *
 *     auto adapter = make_adapter(myArray, elementCount);
 *
 *     // Now you can use the static_vector_adapter like a regular container
 *     std::cout << "Element at index 2: " << adapter[2] << std::endl; // Outputs: Element at index 2: 3
 *
 *     return 0;
 * }
 * @endcode
 */
template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy = BoundCheckStrategy::UB>
static_vector_adapter<T, bc_strategy> make_adapter (std::array<T, SZ>& arr, size_t& count) noexcept
{
    return static_vector_adapter<T, bc_strategy>(arr, count);
}

template<BoundCheckStrategy bc_strategy, typename T, std::size_t SZ>
static_vector_adapter<T, bc_strategy> make_adapter (std::array<T, SZ>& arr, size_t& count) noexcept
{
    return static_vector_adapter<T, bc_strategy>(arr, count);
}

/*! @brief Vector, allocated on stack
 *
 * @tparam T type of stored elements
 * @tparam SZ maximal size
 */
template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy = BoundCheckStrategy::UB>
class static_vector
{
public:
    using value_type      = T;
    using iterator        = value_type*;
    using const_iterator  = const value_type*;
    using size_type       = size_t;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;

    constexpr static_vector( ) = default;

    constexpr explicit static_vector(size_type count);
    constexpr explicit static_vector(size_type count, const_reference value);
    constexpr explicit static_vector(std::initializer_list<value_type> init);
    constexpr explicit static_vector(const static_vector& vec);
    constexpr static_vector(static_vector&& vec) noexcept;

    CXX20_CONSTEXPR ~static_vector( );

    constexpr static_vector& operator= (const static_vector& other);
    constexpr static_vector& operator= (static_vector&& other) noexcept;
    constexpr static_vector& operator= (std::initializer_list<value_type> ilist);

    constexpr void assign(size_type count, const T& value);

    template<std::forward_iterator InputIt>
    constexpr void assign(InputIt first, InputIt last);

    constexpr void assign(std::initializer_list<T> ilist);

    /*!
     * @brief Access specified element with bounds checking
     *
     * Returns a reference to the element at specified location @c pos, with bounds checking.
     *
     * If @c pos is not within the range of the container, an exception of type @c std::out_of_range is thrown
     *
     * \par Exceptions
     * @c std::out_of_range if <tt>pos >= size()</tt>.
     *
     * @param[in] pos position of the element to return
     * @return Reference to the requested element.
     *
     * @par Example
     * @code{.cpp}
        #include <static_vector.h>

        #include <iostream>
        #include <stdexcept>

        #ifdef __GNUG__
        [[gnu::noinline]]
        #endif
        unsigned int runtime_six() // Emulate runtime input
        {
            return 6u;
        }

        int main()
        {
            wbr::static_vector<int, 7> data = {1, 2, 4, 5, 5, 6};

            // Set element 1
            data.at(1) = 88;

            // Read element 2
            std::cout << "Element at index 2 has value " << data.at(2) << '\n';

            std::cout << "data size = " << data.size() << '\n';

            try
            {
                // Set element 6, where the index is determined at runtime
                data.at(runtime_six()) = 666;
            }
            catch (std::out_of_range const& exc)
            {
                std::cout << exc.what() << '\n';
            }

            // Print final values
            std::cout << "data:";
            for (int elem : data)
                std::cout << ' ' << elem;
            std::cout << '\n';
        }
     * @endcode
     * Possible output
     * @code{.terminal}
        Element at index 2 has value 4
        data size = 6
        static_vector::at: pos (which is 6) >= this->size() (which is 6)
        data: 1 88 4 5 5 6
     * @endcode
     * @see operator[]
     * @see <a href=https://en.cppreference.com/w/cpp/container/vector/at>std::vector::at</a>
     */
    constexpr reference at(size_type pos);

    constexpr const_reference at(size_type pos) const;

    /*!
     * @brief Access specified element
     *
     * Returns a reference to the element at specified location pos. No bounds checking is performed.
     *
     * Unlike std::map::operator[], this operator never inserts a new element into the container. Accessing a nonexistent element through this operator is undefined behavior.
     *
     * @param[in] pos position of the element to return
     * @return Reference to the requested element.
     *
     * @par Example
     * The following code uses operator[] to read from and write to a std::vector<int>:
     * @code{.cpp}
     * #include "static_vector.h"
     * #include <iostream>
     *
     * int main() {
     *     wbr::static_vector<int, 4> numbers{2, 4, 6, 8};
     *
     *     std::cout << "Second element: " << numbers[1] << '\n';
     *
     *     numbers[0] = 5;
     *
     *     std::cout << "All numbers:";
     *     for (auto i: numbers)
     *         std::cout << ' ' << i;
     *     std::cout << '\n';
     * }
     * @endcode
     * Output:
     * @code{.terminal}
     * Second element: 4
     * All numbers: 5 4 6 8
     * @endcode
     *
     * @see at
     * @see <a href=https://en.cppreference.com/w/cpp/container/vector/operator_at>std::vector::operator[]</a>
     */
    [[nodiscard]] constexpr reference operator[] (size_t pos) noexcept(bc_strategy != BoundCheckStrategy::Exception);

    [[nodiscard]] constexpr const_reference operator[] (size_t pos) const noexcept(bc_strategy != BoundCheckStrategy::Exception);

    /*!
     * @brief Access the first element
     *
     * Returns a reference to the first element in the container.
     *
     * Calling front on an empty container causes undefined behavior.
     *
     * For a container @c c, the expression @c c.front() is equivalent to @c *c.begin().
     *
     * @return Reference to the first element.
     *
     * @par Example
     * The following code uses front to display the first element of a @c std::vector<char>:
     * @code
        #include <static_vector.h>

        #include <iostream>

        int main()
        {
            wbr::static_vector<char, 6> letters{'a', 'b', 'c', 'd', 'e', 'f'};

            if (!letters.empty())
                std::cout << "The first character is '" << letters.front() << "'.\n";
        }
     * @endcode
     * Output:
     * @code{.terminal}
       The first character is 'a'.
     * @endcode
     *
     * @see back
     * @see <a href=https://en.cppreference.com/w/cpp/container/vector/front>std::vector::front</a>
     */
    [[nodiscard]] constexpr reference front( ) noexcept;

    [[nodiscard]] constexpr const_reference front( ) const noexcept;

    /*!
        @brief access the last element

        Returns a reference to the last element in the container.

        Calling back on an empty container causes undefined behavior.

        For a non-empty container @c c, the expression @c c.back() is equivalent to @c *std::prev(c.end()).
        @return Reference to the last element.

        @par Example
        The following code uses back to display the last element of a std::vector<char>:
        @code{.cpp}
         #include <static_vector.h>

         #include <iostream>

         int main()
         {
             std::vector<char> letters{'a', 'b', 'c', 'd', 'e', 'f'};

             if (!letters.empty())
                  std::cout << "The last character is '" << letters.back() << "'.\n";
         }
        @endcode
         Output:
        @code{.cpp}
         The last character is 'f'.
        @endcode
        @see front
        @see <a href=https://en.cppreference.com/w/cpp/container/vector/back>std::vector::back</a>
    */
    [[nodiscard]] constexpr reference back( ) noexcept;

    [[nodiscard]] constexpr const_reference back( ) const noexcept;

    [[nodiscard]] constexpr value_type* data( ) noexcept;

    [[nodiscard]] constexpr const value_type* data( ) const noexcept;

    [[nodiscard]] constexpr iterator begin( ) noexcept;

    [[nodiscard]] constexpr const_iterator begin( ) const noexcept;

    [[nodiscard]] constexpr const_iterator cbegin( ) const noexcept;

    [[nodiscard]] constexpr iterator end( ) noexcept;

    [[nodiscard]] constexpr const_iterator end( ) const noexcept;

    [[nodiscard]] constexpr const_iterator cend( ) const noexcept;

    [[nodiscard]] constexpr auto rbegin( ) noexcept;

    [[nodiscard]] constexpr auto rbegin( ) const noexcept;

    [[nodiscard]] constexpr auto crbegin( ) const noexcept;

    [[nodiscard]] constexpr auto rend( ) noexcept;

    [[nodiscard]] constexpr auto rend( ) const noexcept;

    [[nodiscard]] constexpr auto crend( ) const noexcept;

    [[nodiscard]] constexpr bool empty( ) const noexcept;

    [[nodiscard]] constexpr size_type size( ) const noexcept;

    [[nodiscard]] constexpr size_type max_size( ) const noexcept;

    constexpr void reserve( );

    [[nodiscard]] constexpr size_type capacity( ) const noexcept;

    constexpr void shrink_to_fit( );

    constexpr void clear( ) noexcept;

    constexpr void push_back(const_reference value);

    constexpr void push_back(value_type&& value);

    template<class... Args>
    constexpr reference emplace_back(Args&&... args);

    /**
     * @brief Removes the last element from the vector.
     *
     * This function removes the last element of the vector, performing a check to ensure
     * that the container is not empty before proceeding. If the value type is not trivially
     * destructible, it explicitly calls the destructor on the last element.
     *
     * @note The function is marked as noexcept because it does not throw exceptions.
     */
    constexpr void pop_back( ) noexcept(bc_strategy != BoundCheckStrategy::Exception);

    constexpr iterator erase(const_iterator pos);

    constexpr iterator erase(const_iterator first, const_iterator last);

    constexpr iterator insert(const_iterator pos, const_reference value);

    constexpr iterator insert(const_iterator pos, value_type&& value);

    constexpr iterator insert(const_iterator pos, size_type count, const_reference value);

    template<std::forward_iterator InputIt>
    constexpr iterator insert(const_iterator pos, InputIt first, InputIt last);

    constexpr iterator insert(const_iterator pos, std::initializer_list<value_type> ilist);

    template<class... Args>
    constexpr iterator emplace(const_iterator pos, Args&&... args);

    constexpr void resize(size_type count, const_reference value);

    constexpr void resize(size_type count);

    constexpr void swap(static_vector& other) noexcept;

private:
    size_type                  elementsCount {0};
    static constexpr size_t    element_size = sizeof(value_type);
    static constexpr size_type bytesize     = SZ * element_size;
    alignas(alignof(value_type)) std::array<std::byte, bytesize> arr;

    constexpr void shift_elements_right(size_type offset, size_type count);
    constexpr void shift_elements_left(size_type offset, size_type count);
};

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::shift_elements_left (size_type offset, size_type count) -> void
{
    std::byte* first  = arr.data( ) + (offset + count) * element_size;
    std::byte* last   = arr.data( ) + elementsCount * element_size;
    std::byte* dfirst = arr.data( ) + offset * element_size;

    if constexpr ( std::is_trivially_copyable_v<T> )
        std::move(first, last, dfirst);
    else
#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
        std::uninitialized_move(exec_policy, begin( ) + offset + count, end( ), begin( ) + offset);
#else
        std::uninitialized_move(begin( ) + offset + count, end( ), begin( ) + offset);
#endif
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::shift_elements_right (static_vector::size_type offset, static_vector::size_type count) -> void
{
    std::byte* first = arr.data( ) + offset * element_size;
    std::byte* last  = arr.data( ) + elementsCount * element_size;
    std::byte* dlast = arr.data( ) + (elementsCount + count) * element_size;

    if constexpr ( std::is_trivially_copyable_v<T> )
        std::move_backward(first, last, dlast);
    else
        uninitialized_move_backward(begin( ) + offset, end( ), end( ) + count);
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
template<class... Args>
constexpr auto static_vector<T, SZ, bc_strategy>::emplace (const_iterator pos, Args&&... args) -> iterator
{
    //    valid_iterator_check(pos);

    const auto offset = std::distance(cbegin( ), pos);

    if ( offset != elementsCount )
        shift_elements_right(offset, 1);
#if __cplusplus >= 202002L
    std::construct_at<value_type>(begin( ) + offset, std::forward<Args>(args)...);
#else
    ::new (begin( ) + offset) value_type(std::forward<Args>(args)...);
#endif
    ++elementsCount;
    return begin( ) + offset;
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::swap (static_vector& other) noexcept -> void
{
    if constexpr ( std::is_trivially_copyable_v<T> ) {
        std::swap(arr, other.arr);
    } else {
        const auto s       = size( );
        const auto other_s = other.size( );
        const auto min     = std::min(s, other_s);
        const auto diff    = std::max(s, other_s) - min;

        std::swap_ranges(begin( ), begin( ) + min, other.begin( ));
        if ( s > other_s )
#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
            std::uninitialized_move_n(exec_policy, begin( ) + min, diff, other.end( ));
        else
            std::uninitialized_move_n(exec_policy, other.begin( ) + min, diff, end( ));
#else
            std::uninitialized_move_n(begin( ) + min, diff, other.end( ));
        else
            std::uninitialized_move_n(other.begin( ) + min, diff, end( ));
#endif
    }
    std::swap(elementsCount, other.elementsCount);
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::resize (static_vector::size_type count) -> void
{
    return resize(count, value_type { });
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::resize (static_vector::size_type count, const_reference value) -> void
{
    if ( count > max_size( ) )
        throw std::length_error {"capacity would exceed max_size()"};
    if ( count > size( ) ) {
        insert(end( ), count - size( ), value);
    } else if ( count < size( ) ) {
        erase(begin( ) + count, end( ));
    }
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::insert (static_vector::const_iterator pos, std::initializer_list<value_type> ilist) -> static_vector::iterator
{
    // count_overflow_check(ilist.size( ));
    // valid_iterator_check(pos);

    return insert(pos, ilist.begin( ), ilist.end( ));
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
template<std::forward_iterator InputIt>
constexpr auto static_vector<T, SZ, bc_strategy>::insert (static_vector::const_iterator pos, InputIt first, InputIt last) -> static_vector::iterator
{
    // count_overflow_check(std::distance(first, last));
    // valid_iterator_check(pos);
    // valid_range_check(first, last);

    const auto offset = std::distance(cbegin( ), pos);
    const auto length = std::distance(first, last);

    if ( offset != elementsCount )
        shift_elements_right(offset, length);
#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
    std::uninitialized_copy(exec_policy, first, last, begin( ) + offset);
#else
    std::uninitialized_copy(first, last, begin( ) + offset);
#endif

    elementsCount += length;
    return begin( ) + offset;
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::insert (static_vector::const_iterator pos, static_vector::size_type count, const_reference value) -> static_vector::iterator
{
    // count_overflow_check(count);
    // valid_iterator_check(pos);

    const auto offset = std::distance(cbegin( ), pos);

    if ( offset != elementsCount )
        shift_elements_right(offset, count);
#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
    std::uninitialized_fill_n(exec_policy, begin( ) + offset, count, value);
#else
    std::uninitialized_fill_n(begin( ) + offset, count, value);
#endif

    elementsCount += count;
    return begin( ) + offset;
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::insert (static_vector::const_iterator pos, value_type&& value) -> static_vector::iterator
{
    // count_overflow_check(1);
    // valid_iterator_check(pos);

    const auto offset = std::distance(cbegin( ), pos);
    if ( offset != elementsCount )
        shift_elements_right(offset, 1);

#if __cplusplus >= 202002L
    std::construct_at<value_type>(begin( ) + offset, std::move(value));
#else
    ::new (begin( ) + offset) value_type(std::move(value));
#endif
    ++elementsCount;
    return begin( ) + offset;
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::insert (static_vector::const_iterator pos, const_reference value) -> static_vector::iterator
{
    // count_overflow_check(1);
    // valid_iterator_check(pos);

    const auto offset = std::distance(cbegin( ), pos);
    if ( offset != elementsCount )
        shift_elements_right(offset, 1);

#if __cplusplus >= 202002L
    std::construct_at<value_type>(begin( ) + offset, value);
#else
    ::new (begin( ) + offset) value_type(value);
#endif
    ++elementsCount;
    return begin( ) + offset;
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::erase (static_vector::const_iterator first, static_vector::const_iterator last) -> static_vector::iterator
{
    // valid_iterator_check(first);
    // valid_iterator_check(last);
    // valid_range_check(first, last);

    const auto offset = std::distance(cbegin( ), first);
    const auto length = std::distance(first, last);

    if constexpr ( !std::is_trivially_destructible_v<value_type> )
        std::destroy_n(begin( ) + offset, length);
    if ( last != cend( ) )
        shift_elements_left(offset, length);

    elementsCount -= length;
    return begin( ) + offset;
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::erase (static_vector::const_iterator pos) -> static_vector::iterator
{
    // valid_iterator_check(pos);

    if ( elementsCount == 0 )
        return end( );

    const auto offset = std::distance(cbegin( ), pos);

    if constexpr ( !std::is_trivially_destructible_v<value_type> )
        std::destroy_at(begin( ) + offset);
    if ( pos != cend( ) - 1 )
        shift_elements_left(offset, 1);
    --elementsCount;
    return begin( ) + offset;
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr void static_vector<T, SZ, bc_strategy>::pop_back ( ) noexcept(bc_strategy != BoundCheckStrategy::Exception)
{
    // not_empty_container_check( );

    if constexpr ( !std::is_trivially_destructible_v<value_type> )
        std::destroy_at(begin( ) + elementsCount - 1);
    --elementsCount;
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
template<class... Args>
constexpr auto static_vector<T, SZ, bc_strategy>::emplace_back (Args&&... args) -> static_vector::reference
{
    // count_overflow_check(1);

#if __cplusplus >= 202002L
    std::construct_at<value_type>(begin( ) + elementsCount, std::forward<Args>(args)...);
#else
    ::new (begin( ) + elementsCount) value_type(std::forward<Args>(args)...);
#endif
    ++elementsCount;
    return back( );
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::push_back (value_type&& value) -> void
{
    // count_overflow_check(1);

    emplace_back(std::move(value));
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::push_back (const_reference value) -> void
{
    // count_overflow_check(1);

#if __cplusplus >= 202002L
    std::construct_at<value_type>(end( ), value);
#else
    ::new (end( )) value_type(value);
#endif
    ++elementsCount;
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::clear ( ) noexcept -> void
{
    if constexpr ( !std::is_trivially_destructible_v<T> )
        std::destroy_n(begin( ), elementsCount);

    elementsCount = 0;
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::shrink_to_fit ( ) -> void
{ /*static allocation -- cannot resize thus no need for shrinking */
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::capacity ( ) const noexcept -> static_vector::size_type
{
    return max_size( );
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::reserve ( ) -> void
{ /*static allocation -- cannot resize thus no need for reservation */
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::max_size ( ) const noexcept -> static_vector::size_type
{
    return SZ;
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::size ( ) const noexcept -> static_vector::size_type
{
    return elementsCount;
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::empty ( ) const noexcept -> bool
{
    return size( ) == 0;
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::crend ( ) const noexcept
{
    return std::make_reverse_iterator(cbegin( ));
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::rend ( ) const noexcept
{
    return std::make_reverse_iterator(begin( ));
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::rend ( ) noexcept
{
    return std::make_reverse_iterator(begin( ));
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::crbegin ( ) const noexcept
{
    return std::make_reverse_iterator(cend( ));
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::rbegin ( ) const noexcept
{
    return std::make_reverse_iterator(end( ));
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::rbegin ( ) noexcept
{
    return std::make_reverse_iterator(end( ));
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::cend ( ) const noexcept -> static_vector::const_iterator
{
    return cbegin( ) + size( );
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::end ( ) const noexcept -> static_vector::const_iterator
{
    return begin( ) + size( );
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::end ( ) noexcept -> static_vector::iterator
{
    return begin( ) + size( );
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::cbegin ( ) const noexcept -> static_vector::const_iterator
{
    return data( );
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::begin ( ) const noexcept -> static_vector::const_iterator
{
    return data( );
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::begin ( ) noexcept -> static_vector::iterator
{
    static_assert(offsetof(decltype(arr), _M_elems) == 0);
#if __cpp_lib_bit_cast >= 201806L
    return std::launder(std::bit_cast<value_type*>(&arr));
#else
    return std::launder(reinterpret_cast<value_type*>(&arr));
#endif
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::data ( ) const noexcept -> static_vector::const_pointer
{
#if __cpp_lib_bit_cast >= 201806L
    return std::launder(std::bit_cast<value_type*>(&arr));
#else
    return std::launder(reinterpret_cast<const value_type*>(&arr));
#endif
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::data ( ) noexcept -> static_vector::pointer
{
#if __cpp_lib_bit_cast >= 201806L
    return std::launder(std::bit_cast<value_type*>(&arr));
#else
    return std::launder(reinterpret_cast<value_type*>(&arr));
#endif
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::back ( ) const noexcept -> static_vector::const_reference
{
    // not_empty_container_check( );

    return *std::prev(end( ));
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::back ( ) noexcept -> static_vector::reference
{
    // not_empty_container_check( );

    return *std::prev(end( ));
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::front ( ) const noexcept -> static_vector::const_reference
{
    // not_empty_container_check( );

    return *begin( );
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::front ( ) noexcept -> static_vector::reference
{
    // not_empty_container_check( );

    return *begin( );
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::operator[] (size_t pos) const noexcept(bc_strategy != BoundCheckStrategy::Exception) -> static_vector::const_reference
{
    // valid_index_check(pos);

    return *(begin( ) + pos);
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::operator[] (size_t pos) noexcept(bc_strategy != BoundCheckStrategy::Exception) -> static_vector::reference
{
    // valid_index_check(pos);

    return *(begin( ) + pos);
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::at (static_vector::size_type pos) const -> static_vector::const_reference
{
    if ( pos >= size( ) )
#if __cpp_lib_format >= 201907L
        throw std::out_of_range {std::format("static_vector::{}: pos (which is {}) >= this->size() (which is {})", __func__, pos, size( ))};
#else
        throw std::out_of_range {"static_vector::range_check: pos >= this->size()"};
#endif
    return operator[] (pos);
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::at (static_vector::size_type pos) -> static_vector::reference
{
    if ( pos >= size( ) )
#if __cpp_lib_format >= 201907L
        throw std::out_of_range {std::format("static_vector::{}: pos (which is {}) >= this->size() (which is {})", __func__, pos, size( ))};
#else
        throw std::out_of_range {"static_vector::range_check: pos >= this->size()"};
#endif
    return operator[] (pos);
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::assign (std::initializer_list<T> ilist) -> void
{
    // count_fit_capacity_check<bc_strategy>(ilist.size( ), capacity( ));

    return assign(ilist.begin( ), ilist.end( ));
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
template<std::forward_iterator InputIt>
constexpr auto static_vector<T, SZ, bc_strategy>::assign (InputIt first, InputIt last) -> void
{
    // count_fit_capacity_check<bc_strategy>(std::distance(first, last), capacity( ));

    clear( );
#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
    std::uninitialized_copy(exec_policy, first, last, begin( ));
#else
    std::uninitialized_copy(first, last, begin( ));
#endif
    elementsCount = std::distance(first, last);
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::assign (static_vector::size_type count, const T& value) -> void
{
    // count_fit_capacity_check<bc_strategy>(count, capacity( ));

    clear( );
#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
    std::uninitialized_fill_n(exec_policy, begin( ), count, value);
#else
    std::uninitialized_fill_n(begin( ), count, value);
#endif
    elementsCount = count;
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::operator= (std::initializer_list<value_type> ilist) -> static_vector&
{
    // count_fit_capacity_check<bc_strategy>(ilist.size( ), capacity( ));

    clear( );
#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
    std::uninitialized_copy(exec_policy, ilist.begin( ), ilist.end( ), begin( ));
#else
    std::uninitialized_copy(ilist.begin( ), ilist.end( ), begin( ));
#endif
    elementsCount = ilist.size( );
    return *this;
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::operator= (static_vector&& other) noexcept -> static_vector&
{
    count_fit_capacity_check<bc_strategy>(other.size( ), capacity( ));

    if ( &other != this ) {
        clear( );
#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
        std::uninitialized_move(exec_policy, other.begin( ), other.end( ), begin( ));
#else
        std::uninitialized_move(other.begin( ), other.end( ), begin( ));
#endif
        elementsCount = other.elementsCount;
    }
    return *this;
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr auto static_vector<T, SZ, bc_strategy>::operator= (const static_vector& other) -> static_vector&
{
    count_fit_capacity_check<bc_strategy>(other.size( ), capacity( ));

    if ( &other != this ) {
        clear( );
#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
        std::uninitialized_copy(exec_policy, other.begin( ), other.end( ), begin( ));
#else
        std::uninitialized_copy(other.begin( ), other.end( ), begin( ));
#endif
        elementsCount = other.elementsCount;
    }
    return *this;
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
CXX20_CONSTEXPR inline static_vector<T, SZ, bc_strategy>::~static_vector( )
{
    clear( );
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr static_vector<T, SZ, bc_strategy>::static_vector(static_vector&& vec) noexcept
{
#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
    std::uninitialized_move(exec_policy, vec.begin( ), vec.end( ), begin( ));
#else
    std::uninitialized_move(vec.begin( ), vec.end( ), begin( ));
#endif
    elementsCount = vec.elementsCount;
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr static_vector<T, SZ, bc_strategy>::static_vector(const static_vector& vec)
{
#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
    std::uninitialized_copy(exec_policy, vec.cbegin( ), vec.cend( ), begin( ));
#else
    std::uninitialized_copy(vec.cbegin( ), vec.cend( ), begin( ));
#endif
    elementsCount = vec.elementsCount;
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr static_vector<T, SZ, bc_strategy>::static_vector(size_type count, const_reference value) : elementsCount(count)
{
    count_fit_capacity_check<bc_strategy>(elementsCount, capacity( ));

#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
    std::uninitialized_fill_n(exec_policy, begin( ), count, value);
#else
    std::uninitialized_fill_n(begin( ), count, value);
#endif
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr static_vector<T, SZ, bc_strategy>::static_vector(size_type count) : elementsCount(count)
{
    count_fit_capacity_check<bc_strategy>(elementsCount, capacity( ));

#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
    std::uninitialized_default_construct_n(exec_policy, begin( ), count);
#else
    std::uninitialized_default_construct_n(begin( ), count);
#endif
}

template<typename T, std::size_t SZ, BoundCheckStrategy bc_strategy>
constexpr static_vector<T, SZ, bc_strategy>::static_vector(std::initializer_list<value_type> init) : elementsCount(init.size( ))
{
    count_fit_capacity_check<bc_strategy>(elementsCount, capacity( ));

#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
    std::uninitialized_copy(exec_policy, init.begin( ), init.end( ), begin( ));
#else
    std::uninitialized_copy(init.begin( ), init.end( ), begin( ));
#endif
}

template<class T, size_t SZ, BoundCheckStrategy astrat, BoundCheckStrategy bstrat>
[[nodiscard]] constexpr bool operator== (const wbr::static_vector<T, SZ, astrat>& lhs, const wbr::static_vector<T, SZ, bstrat>& rhs)
{
    return lhs.size( ) == rhs.size( ) && std::equal(lhs.cbegin( ), lhs.cend( ), rhs.cbegin( ));
}

#if __cpp_lib_three_way_comparison

template<class T, size_t SZ>
[[nodiscard]] constexpr std::strong_ordering operator<=> (const wbr::static_vector<T, SZ>& lhs, const wbr::static_vector<T, SZ>& rhs)
{
    return std::lexicographical_compare_three_way(lhs.cbegin( ), lhs.cend( ), rhs.cbegin( ), rhs.cend( ));
}

#else
template<class T, size_t SZ>
[[nodiscard]] constexpr bool operator!= (const wbr::static_vector<T, SZ>& lhs, const wbr::static_vector<T, SZ>& rhs)
{
    return !(lhs == rhs);
}

template<class T, size_t SZ>
[[nodiscard]] constexpr bool operator< (const wbr::static_vector<T, SZ>& lhs, const wbr::static_vector<T, SZ>& rhs)
{
    return std ::lexicographical_compare(lhs.begin( ), lhs.end( ), rhs.begin( ), rhs.end( ));
}

template<class T, size_t SZ>
[[nodiscard]] constexpr bool operator>= (const wbr::static_vector<T, SZ>& lhs, const wbr::static_vector<T, SZ>& rhs)
{
    return !(lhs < rhs);
}

template<class T, size_t SZ>
[[nodiscard]] constexpr bool operator> (const wbr::static_vector<T, SZ>& lhs, const wbr::static_vector<T, SZ>& rhs)
{
    return std::lexicographical_compare(rhs.begin( ), rhs.end( ), lhs.begin( ), lhs.end( ));
}

template<class T, size_t SZ>
[[nodiscard]] constexpr bool operator<= (const wbr::static_vector<T, SZ>& lhs, const wbr::static_vector<T, SZ>& rhs)
{
    return !(lhs > rhs);
}
#endif
}  // namespace wbr

#ifdef __cpp_lib_erase_if  // C++ >= 20 && HOSTED
namespace std
{
template<class T, size_t SZ>
constexpr auto swap (wbr::static_vector<T, SZ>& a, wbr::static_vector<T, SZ>& b) noexcept(noexcept(a.swap(b))) -> void
{
    a.swap(b);
}

template<class T, size_t SZ, class U = T>
constexpr auto erase (wbr::static_vector<T, SZ>& c, const U& value) -> wbr::static_vector<T, SZ>::size_type
{
    auto it = remove(c.begin( ), c.end( ), value);
    auto r  = c.end( ) - it;
    c.erase(it, c.end( ));
    return r;
}

template<class T, size_t SZ, class Pred>
constexpr auto erase_if (wbr::static_vector<T, SZ>& c, Pred pred) -> wbr::static_vector<T, SZ>::size_type
{
    auto it = std::remove_if(c.begin( ), c.end( ), pred);
    auto r  = c.end( ) - it;
    c.erase(it, c.end( ));
    return r;
}
}  // namespace std
#endif
