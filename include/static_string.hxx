#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <stdexcept>

#include "bound_check.hxx"

#if BUILD_TESTS
    #include <gtest/gtest.h>
#endif

namespace wbr
{

/**
 * @class static_string_adapter
 *
 * @brief A simple adapter class for managing a C-style string with fixed capacity.
 *
 * This class provides an interface for working with character arrays as if they were strings,
 * with methods to manage the length and ensure proper null-termination.
 */
template<BoundCheckStrategy bc_strategy = BoundCheckStrategy::NoCheck>
class static_string_adapter
{
public:
    using char_type                 = char;              ///< The type of characters used in this string (char)
    using pointer                   = char_type*;        ///< Pointer to a char_type (mutable)
    using const_pointer             = const char_type*;  ///< Pointer to a const char_type (immutable)
    using size_type                 = std::size_t;       ///< Type for representing sizes and indices
    using iterator                  = pointer;
    using const_iterator            = const_pointer;
    using reference                 = char_type&;
    using const_reference           = const char_type&;
    static constexpr size_type npos = static_cast<size_type>(-1);

    /**
     * @brief Constructs an empty string adapter with the given array capacity.
     *
     * @param array Pointer to the character array
     * @param array_size The maximum number of characters that can be stored in the array (including null terminator)
     */
    static_string_adapter (char_type* array, std::size_t array_size) : head_ {array}, tail_ {head_}, max_length_ {array_size - 1}
    {
        using enum BoundCheckStrategy;
        if constexpr ( bc_strategy == Assert ) {
            assert(array != nullptr);
            assert(array_size > 0);
        }
        if constexpr ( bc_strategy == Exception ) {
            if ( array == nullptr )
                throw std::runtime_error {"null pointer"};
            if ( array_size == 0 )
                throw std::length_error {"zero-length array"};
        }
        if constexpr ( bc_strategy == LimitToBound )
            return;  // will remain empty forever

        *tail_ = '\0';
    }

    /**
     * @brief Constructs a string adapter with a specified initial string length.
     *
     * @param array Pointer to the character array
     * @param array_size The maximum number of characters that can be stored in the array (including null terminator)
     * @param string_length Initial length of the string
     */
    static_string_adapter (char_type* array, std::size_t array_size, std::size_t string_length) : head_ {array}, tail_ {head_ + string_length}, max_length_ {array_size - 1}
    {
        using enum BoundCheckStrategy;
        if constexpr ( bc_strategy == Assert ) {
            assert(array_size > 0);
            assert(string_length <= max_length_);
        }
        if constexpr ( bc_strategy == Exception ) {
            if ( array == nullptr )
                throw std::runtime_error {"null pointer"};
            if ( array_size == 0 )
                throw std::length_error {"zero-length array"};
        }
        if constexpr ( bc_strategy == LimitToBound )
            return;  // will remain empty forever;

        *tail_ = '\0';
    }

    /**
     * @brief Constructs an empty string adapter from a std::array.
     *
     * @tparam SZ Size of the array
     * @param array Reference to the std::array
     */
    template<std::size_t SZ>
    static_string_adapter(std::array<char_type, SZ>& array) : static_string_adapter(array.data( ), array.size( ))

    {
    }

    /**
     * @brief Constructs an empty string adapter from a raw C-style array.
     *
     * @tparam SZ Size of the array
     * @param array Reference to the C-style array
     */
    template<std::size_t SZ>
    static_string_adapter(char_type (&array)[SZ]) : static_string_adapter(array, SZ)
    {
    }

    /**
     * @brief Returns the length of the string.
     *
     * This method returns the current number of characters in the string,
     * not counting the null terminator.
     *
     * @throws  Does not throw exceptions.
     *
     * @return The length of the string as a size_type.
     */
    [[nodiscard]] constexpr size_type length ( ) const noexcept { return std::distance(head_, tail_); }

    /**
     * @brief Returns the length of the string.
     *
     * This method returns the current number of characters in the string,
     * not counting the null terminator. It is an alias for the length() method.
     * @throws  Does not throw exceptions.
     * @return The size of the string as a size_type.
     */
    [[nodiscard]] constexpr size_type size ( ) const noexcept { return length( ); }

    /**
     * @brief Returns the maximum size of the string.
     *
     * This method returns the maximum number of characters that can be stored
     * in the string, not counting the null terminator.
     * @throws  Does not throw exceptions.
     * @return The maximum size of the string as a size_type.
     */
    [[nodiscard]] constexpr size_type max_size ( ) const noexcept { return max_length_; }

    /**
     * @brief Accesses the character at the specified position with bounds checking.
     *
     * This method provides access to the character at the given index. It does not modify
     * any internal state of the object. Never perform bound check and never throw exception.
     * Undefined behavior if `pos` is greater then length of the string.
     *
     * @throws Does not throws exceptions.
     *
     * @param pos The index of the character to access.
     * @return A constant reference to the character at position `pos`.
     */
    [[nodiscard]] const_reference operator[] (size_type pos) const noexcept { return *(head_ + pos); }

    /**
     * @brief Accesses the character at the specified position with bounds checking.
     *
     * This method provides access to the character at the given index. It does not modify
     * any internal state of the object. Never perform bounds and never throw expection. Undefined behavior
     * if `pos` is greater then length of the string.
     *
     * @param pos The index of the character to access.
     * @return A reference to the character at position `pos`.
     */
    [[nodiscard]] reference operator[] (size_type pos) noexcept { return *(head_ + pos); }

    void clear ( ) noexcept
    {
        tail_  = head_;
        *tail_ = '\0';
    }

    [[nodiscard]] const_pointer c_str ( ) const noexcept { return head_; }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& append (std::input_iterator auto first, std::input_iterator auto last) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception)
    {
        auto n = std::distance(first, last);
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Exception ) {
            if ( first > last )
                throw std::runtime_error("iterators mismatch (first > last");
            if ( size( ) + n > max_size( ) )
                throw std::overflow_error("too long");
        }
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Assert ) {
            assert(first <= last);
            assert(size( ) + n <= max_size( ));
        }
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::LimitToBound ) {
            n = std::min(static_cast<size_type>(std::abs(n)), max_size( ) - size( ));
        }

        std::copy_n(first, n, tail_);
        tail_ += n;
        *tail_ = '\0';
        return *this;
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& append (size_type count, char c) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception)
    {
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Exception ) {
            if ( count + size( ) > max_size( ) )
                throw std::overflow_error("too long");
        }
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Assert ) {
            assert(count + size( ) <= max_size( ));
        }
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::LimitToBound )
            count = std::min(count, max_size( ) - size( ));

        std::fill_n(tail_, count, c);
        tail_ += count;
        *tail_ = '\0';
        return *this;
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& append (char c) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception)
    {
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Exception ) {
            if ( size( ) == max_size( ) )
                throw std::overflow_error("too long");
        }
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Assert ) {
            assert(size( ) < max_size( ));
        }
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::LimitToBound )
            return *this;

        *tail_ = c;
        ++tail_;
        *tail_ = '\0';
        return *this;
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& append (std::string_view sv) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception)
    {
        return append<custom_bc_strategy>(sv.cbegin( ), sv.cend( ));
    }

    /** https://en.cppreference.com/w/cpp/string/basic_string/assign */
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& assign (std::input_iterator auto first, std::input_iterator auto last) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception)
    {
        clear( );
        return append<custom_bc_strategy>(first, last);
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& assign (size_type count, char c) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception)
    {
        clear( );
        return append<custom_bc_strategy>(count, c);
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& assign (char c) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception)
    {
        clear( );
        return append(c);
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& assign (std::string_view sv) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception)
    {
        return assign<custom_bc_strategy>(sv.cbegin( ), sv.cend( ));
    }

    /*
    constexpr static_string_adapter& assign(std::string_view sv, size_type pos, size_type count = npos);
    constexpr static_string_adapter& assign(const char* str);
    constexpr static_string_adapter& assign(const char* str, size_type count);
    constexpr static_string_adapter& assign(std::initializer_list<char> ilist);
     */

    [[nodiscard]] constexpr iterator begin ( ) noexcept { return head_; }

private:
    const pointer   head_ {nullptr};  ///< Pointer to the beginning of the string
    pointer         tail_ {nullptr};  ///< Pointer to one past the end of the string (points to null terminator)
    const size_type max_length_;      ///< Maximum allowed length for the string (excluding null terminator)

#if BUILD_TESTS
    FRIEND_TEST(StaticStringAdapterTest, DefaultConstructor);
    FRIEND_TEST(StaticStringAdapterTest, ConstructorWithLength);
    FRIEND_TEST(StaticStringAdapterTest, ArrayConstructor);
    FRIEND_TEST(StaticStringAdapterTest, CArrayConstructor);
#endif
};

template<std::size_t SZ, BoundCheckStrategy bc_strategy = BoundCheckStrategy::NoCheck>
class static_string : static_string_adapter<bc_strategy>
{
    using parent = static_string_adapter<bc_strategy>;

    static_string ( ) : parent {data_.data( ), SZ, 0} { }

private:
    std::array<typename parent::char_type, SZ> data_;
};

}  // namespace wbr
