#pragma once

#include <algorithm>
#include <array>

#if __cplusplus >= 202002L

    #include <compare>

#endif

#include <cstring>
#include <initializer_list>
#include <memory>
#include <stdexcept>

#if __cpp_lib_format >= 201907L

    #include <format>

#endif

#if !defined(wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS)
    #define wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS 0
#endif

#if !defined(wbr_STATIC_VECTOR_DO_RANGE_CHECKS)
    #define wbr_STATIC_VECTOR_DO_RANGE_CHECKS 0
#endif

#if defined(NDEBUG)
    #undef wbr_STATIC_VECTOR_DO_RANGE_CHECKS
    #define wbr_STATIC_VECTOR_DO_RANGE_CHECKS 0
#endif

#if wbr_STATIC_VECTOR_DO_RANGE_CHECKS
    #include <cassert>
    #define valid_index_check(idx_)          assert(0 <= idx_ && idx_ < this->size( ))
    #define not_empty_container_check( )     assert(!this->empty( ))
    #define count_overflow_check(count_)     assert(this->elementsCount + count_ <= this->max_size( ))
    #define count_fit_capacity_check(count_) assert(count_ <= this->max_size( ))
    #define valid_iterator_check(iter_)      assert(this->cbegin( ) <= iter_ && iter_ <= this->cend( ))
    #define valid_range_check(first_, last_) assert(first_ <= last_)

#else
    #define valid_index_check(idx_)
    #define not_empty_container_check( )
    #define count_overflow_check(count_)
    #define count_fit_capacity_check(count_)
    #define valid_iterator_check(iter_)
    #define valid_range_check(first_, last_)

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

/*! @brief Vector, allocated on stack
 *
 * @tparam T type of stored elements
 * @tparam SZ maximal size
 */
template<typename T, std::size_t SZ>
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
    constexpr explicit static_vector(const static_vector<value_type, SZ>& vec);
    constexpr static_vector(static_vector&& vec) noexcept;

    CXX20_CONSTEXPR ~static_vector( );

    constexpr static_vector& operator= (const static_vector& other);
    constexpr static_vector& operator= (static_vector&& other) noexcept;
    constexpr static_vector& operator= (std::initializer_list<value_type> ilist);

    constexpr void assign(size_type count, const T& value);

    template<class InputIt>
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
    [[nodiscard]] constexpr reference operator[] (size_t pos) noexcept;

    [[nodiscard]] constexpr const_reference operator[] (size_t pos) const noexcept;

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
    constexpr void pop_back( ) noexcept;

    constexpr iterator erase(const_iterator pos);

    constexpr iterator erase(const_iterator first, const_iterator last);

    constexpr iterator insert(const_iterator pos, const_reference value);

    constexpr iterator insert(const_iterator pos, value_type&& value);

    constexpr iterator insert(const_iterator pos, size_type count, const_reference value);

    template<class InputIt>
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

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::shift_elements_left (static_vector::size_type offset, static_vector::size_type count) -> void
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

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::shift_elements_right (static_vector::size_type offset, static_vector::size_type count) -> void
{
    std::byte* first = arr.data( ) + offset * element_size;
    std::byte* last  = arr.data( ) + elementsCount * element_size;
    std::byte* dlast = arr.data( ) + (elementsCount + count) * element_size;

    if constexpr ( std::is_trivially_copyable_v<T> )
        std::move_backward(first, last, dlast);
    else
        uninitialized_move_backward(begin( ) + offset, end( ), end( ) + count);
}

template<typename T, std::size_t SZ>
template<class... Args>
constexpr auto static_vector<T, SZ>::emplace (const_iterator pos, Args&&... args) -> iterator
{
    count_overflow_check(1);
    valid_iterator_check(pos);

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

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::swap (static_vector& other) noexcept -> void
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

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::resize (static_vector::size_type count) -> void
{
    return resize(count, value_type { });
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::resize (static_vector::size_type count, const_reference value) -> void
{
    if ( count > max_size( ) )
        throw std::length_error {"capacity would exceed max_size()"};
    if ( count > size( ) ) {
        insert(end( ), count - size( ), value);
    } else if ( count < size( ) ) {
        erase(begin( ) + count, end( ));
    }
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::insert (static_vector::const_iterator pos, std::initializer_list<value_type> ilist) -> static_vector<T, SZ>::iterator
{
    count_overflow_check(ilist.size( ));
    valid_iterator_check(pos);

    return insert(pos, ilist.begin( ), ilist.end( ));
}

template<typename T, std::size_t SZ>
template<class InputIt>
constexpr auto static_vector<T, SZ>::insert (static_vector::const_iterator pos, InputIt first, InputIt last) -> static_vector<T, SZ>::iterator
{
    count_overflow_check(std::distance(first, last));
    valid_iterator_check(pos);
    valid_range_check(first, last);

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

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::insert (static_vector::const_iterator pos, static_vector::size_type count, const_reference value) -> static_vector<T, SZ>::iterator
{
    count_overflow_check(count);
    valid_iterator_check(pos);

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

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::insert (static_vector::const_iterator pos, value_type&& value) -> static_vector<T, SZ>::iterator
{
    count_overflow_check(1);
    valid_iterator_check(pos);

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

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::insert (static_vector::const_iterator pos, const_reference value) -> static_vector<T, SZ>::iterator
{
    count_overflow_check(1);
    valid_iterator_check(pos);

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

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::erase (static_vector::const_iterator first, static_vector::const_iterator last) -> static_vector<T, SZ>::iterator
{
    valid_iterator_check(first);
    valid_iterator_check(last);
    valid_range_check(first, last);

    const auto offset = std::distance(cbegin( ), first);
    const auto length = std::distance(first, last);

    if constexpr ( !std::is_trivially_destructible_v<value_type> )
        std::destroy_n(begin( ) + offset, length);
    if ( last != cend( ) )
        shift_elements_left(offset, length);

    elementsCount -= length;
    return begin( ) + offset;
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::erase (static_vector::const_iterator pos) -> static_vector<T, SZ>::iterator
{
    valid_iterator_check(pos);

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

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::pop_back ( ) noexcept -> void
{
    not_empty_container_check( );

    if constexpr ( !std::is_trivially_destructible_v<value_type> )
        std::destroy_at(begin( ) + elementsCount - 1);
    --elementsCount;
}

template<typename T, std::size_t SZ>
template<class... Args>
constexpr auto static_vector<T, SZ>::emplace_back (Args&&... args) -> static_vector<T, SZ>::reference
{
    count_overflow_check(1);

#if __cplusplus >= 202002L
    std::construct_at<value_type>(begin( ) + elementsCount, std::forward<Args>(args)...);
#else
    ::new (begin( ) + elementsCount) value_type(std::forward<Args>(args)...);
#endif
    ++elementsCount;
    return back( );
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::push_back (value_type&& value) -> void
{
    count_overflow_check(1);

    emplace_back(std::move(value));
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::push_back (const_reference value) -> void
{
    count_overflow_check(1);

#if __cplusplus >= 202002L
    std::construct_at<value_type>(end( ), value);
#else
    ::new (end( )) value_type(value);
#endif
    ++elementsCount;
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::clear ( ) noexcept -> void
{
    if constexpr ( !std::is_trivially_destructible_v<T> )
        std::destroy_n(begin( ), elementsCount);

    elementsCount = 0;
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::shrink_to_fit ( ) -> void
{ /*static allocation -- cannot resize thus no need for shrinking */
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::capacity ( ) const noexcept -> static_vector<T, SZ>::size_type
{
    return max_size( );
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::reserve ( ) -> void
{ /*static allocation -- cannot resize thus no need for reservation */
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::max_size ( ) const noexcept -> static_vector<T, SZ>::size_type
{
    return SZ;
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::size ( ) const noexcept -> static_vector<T, SZ>::size_type
{
    return elementsCount;
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::empty ( ) const noexcept -> bool
{
    return size( ) == 0;
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::crend ( ) const noexcept
{
    return std::make_reverse_iterator(cbegin( ));
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::rend ( ) const noexcept
{
    return std::make_reverse_iterator(begin( ));
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::rend ( ) noexcept
{
    return std::make_reverse_iterator(begin( ));
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::crbegin ( ) const noexcept
{
    return std::make_reverse_iterator(cend( ));
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::rbegin ( ) const noexcept
{
    return std::make_reverse_iterator(end( ));
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::rbegin ( ) noexcept
{
    return std::make_reverse_iterator(end( ));
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::cend ( ) const noexcept -> static_vector<T, SZ>::const_iterator
{
    return cbegin( ) + size( );
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::end ( ) const noexcept -> static_vector<T, SZ>::const_iterator
{
    return begin( ) + size( );
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::end ( ) noexcept -> static_vector<T, SZ>::iterator
{
    return begin( ) + size( );
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::cbegin ( ) const noexcept -> static_vector<T, SZ>::const_iterator
{
    return data( );
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::begin ( ) const noexcept -> static_vector<T, SZ>::const_iterator
{
    return data( );
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::begin ( ) noexcept -> static_vector<T, SZ>::iterator
{
    static_assert(offsetof(decltype(arr), _M_elems) == 0);
#if __cpp_lib_bit_cast >= 201806L
    return std::launder(std::bit_cast<value_type*>(&arr));
#else
    return std::launder(reinterpret_cast<value_type*>(&arr));
#endif
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::data ( ) const noexcept -> const static_vector<T, SZ>::value_type*
{
#if __cpp_lib_bit_cast >= 201806L
    return std::launder(std::bit_cast<value_type*>(&arr));
#else
    return std::launder(reinterpret_cast<const value_type*>(&arr));
#endif
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::data ( ) noexcept -> static_vector<T, SZ>::value_type*
{
#if __cpp_lib_bit_cast >= 201806L
    return std::launder(std::bit_cast<value_type*>(&arr));
#else
    return std::launder(reinterpret_cast<value_type*>(&arr));
#endif
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::back ( ) const noexcept -> static_vector<T, SZ>::const_reference
{
    not_empty_container_check( );

    return *std::prev(end( ));
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::back ( ) noexcept -> static_vector<T, SZ>::reference
{
    not_empty_container_check( );

    return *std::prev(end( ));
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::front ( ) const noexcept -> static_vector<T, SZ>::const_reference
{
    not_empty_container_check( );

    return *begin( );
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::front ( ) noexcept -> static_vector<T, SZ>::reference
{
    not_empty_container_check( );

    return *begin( );
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::operator[] (size_t pos) const noexcept -> static_vector<T, SZ>::const_reference
{
    valid_index_check(pos);

    return *(begin( ) + pos);
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::operator[] (size_t pos) noexcept -> static_vector<T, SZ>::reference
{
    valid_index_check(pos);

    return *(begin( ) + pos);
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::at (static_vector::size_type pos) const -> static_vector<T, SZ>::const_reference
{
    if ( pos >= size( ) )
#if __cpp_lib_format >= 201907L
        throw std::out_of_range {std::format("static_vector::{}: pos (which is {}) >= this->size() (which is {})", __func__, pos, size( ))};
#else
        throw std::out_of_range {"static_vector::range_check: pos >= this->size()"};
#endif
    return operator[] (pos);
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::at (static_vector::size_type pos) -> static_vector<T, SZ>::reference
{
    if ( pos >= size( ) )
#if __cpp_lib_format >= 201907L
        throw std::out_of_range {std::format("static_vector::{}: pos (which is {}) >= this->size() (which is {})", __func__, pos, size( ))};
#else
        throw std::out_of_range {"static_vector::range_check: pos >= this->size()"};
#endif
    return operator[] (pos);
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::assign (std::initializer_list<T> ilist) -> void
{
    count_fit_capacity_check(ilist.size( ));

    return assign(ilist.begin( ), ilist.end( ));
}

template<typename T, std::size_t SZ>
template<class InputIt>
constexpr auto static_vector<T, SZ>::assign (InputIt first, InputIt last) -> void
{
    count_fit_capacity_check(std::distance(first, last));

    clear( );
#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
    std::uninitialized_copy(exec_policy, first, last, begin( ));
#else
    std::uninitialized_copy(first, last, begin( ));
#endif
    elementsCount = std::distance(first, last);
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::assign (static_vector::size_type count, const T& value) -> void
{
    count_fit_capacity_check(count);

    clear( );
#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
    std::uninitialized_fill_n(exec_policy, begin( ), count, value);
#else
    std::uninitialized_fill_n(begin( ), count, value);
#endif
    elementsCount = count;
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::operator= (std::initializer_list<value_type> ilist) -> static_vector<T, SZ>&
{
    count_fit_capacity_check(ilist.size( ));

    clear( );
#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
    std::uninitialized_copy(exec_policy, ilist.begin( ), ilist.end( ), begin( ));
#else
    std::uninitialized_copy(ilist.begin( ), ilist.end( ), begin( ));
#endif
    elementsCount = ilist.size( );
    return *this;
}

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::operator= (static_vector&& other) noexcept -> wbr::static_vector<T, SZ>&
{
    count_fit_capacity_check(other.size( ));

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

template<typename T, std::size_t SZ>
constexpr auto static_vector<T, SZ>::operator= (const static_vector& other) -> static_vector<T, SZ>&
{
    count_fit_capacity_check(other.size( ));

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

template<typename T, std::size_t SZ>
CXX20_CONSTEXPR inline static_vector<T, SZ>::~static_vector( )
{
    clear( );
}

template<typename T, std::size_t SZ>
constexpr static_vector<T, SZ>::static_vector(static_vector&& vec) noexcept
{
#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
    std::uninitialized_move(exec_policy, vec.begin( ), vec.end( ), begin( ));
#else
    std::uninitialized_move(vec.begin( ), vec.end( ), begin( ));
#endif
    elementsCount = vec.elementsCount;
}

template<typename T, std::size_t SZ>
constexpr static_vector<T, SZ>::static_vector(const static_vector& vec)
{
#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
    std::uninitialized_copy(exec_policy, vec.cbegin( ), vec.cend( ), begin( ));
#else
    std::uninitialized_copy(vec.cbegin( ), vec.cend( ), begin( ));
#endif
    elementsCount = vec.elementsCount;
}

template<typename T, std::size_t SZ>
constexpr static_vector<T, SZ>::static_vector(size_type count, const_reference value)
{
    count_fit_capacity_check(count);

#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
    std::uninitialized_fill_n(exec_policy, begin( ), count, value);
#else
    std::uninitialized_fill_n(begin( ), count, value);
#endif
    elementsCount = count;
}

template<typename T, std::size_t SZ>
constexpr static_vector<T, SZ>::static_vector(size_type count)
{
    count_fit_capacity_check(count);

#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
    std::uninitialized_default_construct_n(exec_policy, begin( ), count);
#else
    std::uninitialized_default_construct_n(begin( ), count);
#endif
    elementsCount = count;
}

template<typename T, std::size_t SZ>
constexpr static_vector<T, SZ>::static_vector(std::initializer_list<value_type> init)
{
    count_fit_capacity_check(init.size( ));

#if wbr_STATIC_VECTOR_USE_PARALLEL_ALGORITHMS
    std::uninitialized_copy(exec_policy, init.begin( ), init.end( ), begin( ));
#else
    std::uninitialized_copy(init.begin( ), init.end( ), begin( ));
#endif
    elementsCount = init.size( );
}

template<class T, size_t SZ>
[[nodiscard]] constexpr bool operator== (const wbr::static_vector<T, SZ>& lhs, const wbr::static_vector<T, SZ>& rhs)
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
