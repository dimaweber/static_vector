#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <iterator>
#include <stdexcept>

#include "bound_check.hxx"

#if BUILD_TESTS
    #include <gtest/gtest.h>
#endif

namespace wbr {

template<typename SV>
concept StringViewLike = std::is_convertible_v<const SV&, std::string_view> && !std::is_convertible_v<const SV&, const char*>;

template<typename I>
concept IndexLike = std::is_integral_v<I> && !std::is_pointer_v<I>;

/**
 * @class static_string_adapter
 *
 * @brief A simple adapter class for managing a C-style string with fixed capacity.
 *
 * This class provides an interface for working with character arrays as if they were strings,
 * with methods to manage the length and ensure proper null-termination.
 */
template<BoundCheckStrategy bc_strategy = BoundCheckStrategy::NoCheck>
class static_string_adapter {
public:
    /// @name nested types
    ///@{
    using char_type     = char;              ///< The type of characters used in this string (char)
    using pointer       = char_type*;        ///< Pointer to a char_type (mutable)
    using const_pointer = const char_type*;  ///< Pointer to a const char_type (immutable)
    using size_type     = std::size_t;       ///< Type for representing sizes and indices
    using iterator      = char_type*;
#if __cpp_lib_ranges_as_const > 202207L
    using const_iterator = std::const_iterator<iterator>;
#else
    using const_iterator = const char_type*;
#endif
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reference              = char_type&;
    using const_reference        = const char_type&;

    static_assert(std::contiguous_iterator<iterator>);
    static_assert(IndexLike<size_type>);
    ///@}

    static constexpr size_type npos = static_cast<size_type>(-1);

    /// @name constructor
    /// @{
    /**
     * @brief Constructs an empty string adapter with the given array capacity.
     *
     * @param array Pointer to the character array
     * @param array_size The maximum number of characters that can be stored in the array (including null terminator)
     */
    static_string_adapter (char_type* array, std::size_t array_size) : head_ {array}, tail_ {head_}, max_length_ {array_size - 1} {
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
            if ( array_size == 0 )
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
    static_string_adapter (char_type* array, std::size_t array_size, std::size_t string_length) : head_ {array}, tail_ {head_ + string_length}, max_length_ {array_size - 1} {
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
    static_string_adapter(std::array<char_type, SZ>& array) : static_string_adapter(array.data( ), array.size( )) {
    }

    /**
     * @brief Constructs an empty string adapter from a raw C-style array.
     *
     * @tparam SZ Size of the array
     * @param array Reference to the C-style array
     */
    template<std::size_t SZ>
    static_string_adapter(char_type (&array)[SZ]) : static_string_adapter(array, SZ) {
    }

    /// @}

    ///@name capacity
    ///@{
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
    [[nodiscard]] constexpr size_type length ( ) const noexcept {
        return std::distance(head_, tail_);
    }

    /**
     * @brief Returns the length of the string.
     *
     * This method returns the current number of characters in the string,
     * not counting the null terminator. It is an alias for the length() method.
     * @throws  Does not throw exceptions.
     * @return The size of the string as a size_type.
     */
    [[nodiscard]] constexpr size_type size ( ) const noexcept {
        return length( );
    }

    /**
     * @brief Returns the maximum size of the string.
     *
     * This method returns the maximum number of characters that can be stored
     * in the string, not counting the null terminator.
     * @throws  Does not throw exceptions.
     * @return The maximum size of the string as a size_type.
     */
    [[nodiscard]] constexpr size_type max_size ( ) const noexcept {
        return max_length_;
    }

    [[nodiscard]] constexpr size_type capacity ( ) const noexcept {
        return max_size( );
    }

    [[nodiscard]] constexpr bool empty ( ) const noexcept {
        return head_ == tail_;
    }

    [[nodiscard]] constexpr size_type free_space ( ) const noexcept {
        return max_size( ) - size( );
    }

    ///@}

    /// @name element access
    /// @{
    /**
     * @brief Accesses the character at the specified position without bounds checking.
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
    [[nodiscard]] const_reference operator[] (IndexLike auto pos) const noexcept {
        return *(head_ + pos);
    }

    /**
     * @brief Accesses the character at the specified position without bounds checking.
     *
     * This method provides access to the character at the given index. It does not modify
     * any internal state of the object. Never perform bounds and never throw exception. Undefined behavior
     * if `pos` is greater then length of the string.
     *
     * @param pos The index of the character to access.
     * @return A reference to the character at position `pos`.
     */
    [[nodiscard]] reference operator[] (IndexLike auto pos) noexcept {
        return *(head_ + pos);
    }

    [[nodiscard]] constexpr const_pointer c_str ( ) const noexcept {
        return head_;
    }

    [[nodiscard]] constexpr operator std::string_view ( ) const noexcept {
        return view( );
    }

    [[nodiscard]] constexpr std::string_view view ( ) const noexcept {
        return std::string_view {head_, length( )};
    }

    [[nodiscard]] constexpr pointer data ( ) noexcept {
        return head_;
    }

    [[nodiscard]] constexpr const_pointer data ( ) const noexcept {
        return head_;
    }

    [[nodiscard]] constexpr reference front ( ) {
        return *begin( );
    }

    [[nodiscard]] constexpr const_reference front ( ) const {
        return *cbegin( );
    }

    [[nodiscard]] constexpr reference back ( ) {
        return *(std::prev(end( )));
    }

    [[nodiscard]] constexpr const_reference back ( ) const {
        return *(std::prev(cend( )));
    }

    /// @}

    /// @name modifiers
    ///@{
    void clear ( ) noexcept {
        tail_  = head_;
        *tail_ = '\0';
    }

    // https://en.cppreference.com/w/cpp/string/basic_string/append.html
    // 1
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& append (size_type count, char ch) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        insert<custom_bc_strategy>(cend( ), count, ch);
        return *this;
    }

    // 1a
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& append (char c) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        return append<custom_bc_strategy>(1, c);
    }

    // 2
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& append (const char_type* s, size_type count) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        return append<custom_bc_strategy>(std::string_view {s, count});
    }

    // 3
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& append (const char_type* s) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        return append<custom_bc_strategy>(s, ::strlen(s));
    }

    // 4
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& append (const StringViewLike auto& t) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        const std::string_view sv = t;
        return append<custom_bc_strategy>(sv.cbegin( ), sv.cend( ));
    }

    // 5
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& append (const StringViewLike auto& t, IndexLike auto pos, size_type count = npos) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        const std::string_view sv = t;
        return append<custom_bc_strategy>(sv.substr(pos, count));
    }

    // 6
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& append (const std::string& str) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        return append<custom_bc_strategy>(str.data( ), str.size( ));
    }

    // 7
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& append (const std::string& str, IndexLike auto pos, size_type count = npos) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        const std::string_view sv {str};
        return append<custom_bc_strategy>(sv.substr(pos, count));
    }

    // 8
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& append (std::input_iterator auto first, std::input_iterator auto last) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        insert<custom_bc_strategy>(end( ), first, last);
        return *this;
    }

    // 9
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& append (std::initializer_list<char> ilist) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        return append<custom_bc_strategy>(ilist.begin( ), ilist.end( ));
    }

    constexpr static_string_adapter& operator+= (const std::string& str) noexcept(noexcept(append(str))) {
        return append(str);
    }

    constexpr static_string_adapter& operator+= (const StringViewLike auto& sv) noexcept(noexcept(append(sv))) {
        return append(sv);
    }

    constexpr static_string_adapter& operator+= (const char* cstr) noexcept(noexcept(append(cstr))) {
        return append(cstr);
    }

    constexpr static_string_adapter& operator+= (char ch) noexcept(noexcept(append(ch))) {
        return append(ch);
    }

    constexpr static_string_adapter& operator+= (std::initializer_list<char> ilist) noexcept(noexcept(append(ilist))) {
        return append(ilist);
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr void push_back (char ch) noexcept(noexcept(append(ch))) {
        append(ch);
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr void pop_back ( ) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        erase(std::prev(end( )));
    }

    /// @see https://en.cppreference.com/w/cpp/string/basic_string/assign
    // 1
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& assign (const std::string& str) {
        clear( );
        return append<custom_bc_strategy>(str);
    }

    // 3
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& assign (size_type count, char ch) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        clear( );
        return append<custom_bc_strategy>(count, ch);
    }

    // 4
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& assign (const char_type* cstr, size_type count) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        clear( );
        return append<custom_bc_strategy>(cstr, count);
    }

    // 5
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& assign (const char_type* cstr) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        clear( );
        return append<custom_bc_strategy>(cstr);
    }

    // 6
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& assign (const StringViewLike auto& sv) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        clear( );
        return append<custom_bc_strategy>(sv);
    }

    // 7
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& assign (const StringViewLike auto& sv, IndexLike auto pos, size_type count = npos) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        clear( );
        return append<custom_bc_strategy>(sv, pos, count);
    }

    // 8
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& assign (const std::string& str, IndexLike auto pos, size_type count = npos) {
        clear( );
        return append<custom_bc_strategy>(str, pos, count);
    }

    // 9
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& assign (std::input_iterator auto first, std::input_iterator auto last) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        clear( );
        return append<custom_bc_strategy>(first, last);
    }

    // 10
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& assign (std::initializer_list<char> ilist) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        clear( );
        return append<custom_bc_strategy>(ilist);
    }

    // 11
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& assign (char_type c) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        clear( );
        return append(c);
    }

    // 1
    constexpr static_string_adapter& operator= (const std::string& sv) noexcept(bc_strategy != BoundCheckStrategy::Exception) {
        return assign(sv);
    }

    // 3
    constexpr static_string_adapter& operator= (const char_type* cstr) noexcept(bc_strategy != BoundCheckStrategy::Exception) {
        return assign(cstr);
    }

    // 4
    constexpr static_string_adapter& operator= (char_type ch) noexcept(bc_strategy != BoundCheckStrategy::Exception) {
        return assign(ch);
    }

    // 5
    constexpr static_string_adapter& operator= (std::initializer_list<char_type> ilist) noexcept(bc_strategy != BoundCheckStrategy::Exception) {
        return assign(ilist);
    }

    // 6
    constexpr static_string_adapter& operator= (const StringViewLike auto& sv) noexcept(bc_strategy != BoundCheckStrategy::Exception) {
        return assign(sv);
    }

    // 7
    static_string_adapter& operator= (std::nullptr_t) = delete;

    // 1
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& insert (IndexLike auto index, size_type count, char ch) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        insert<custom_bc_strategy>(std::next(begin( ), index), count, ch);
        return *this;
    }

    // 1a
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& insert (IndexLike auto index, char ch) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        insert<custom_bc_strategy>(std::next(begin( ), index), ch);
        return *this;
    }

    // 2
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& insert (IndexLike auto index, const char_type* cstr) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        return insert<custom_bc_strategy>(index, std::string_view {cstr});
    }

    // 3
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& insert (IndexLike auto index, const char_type* cstr, size_type count) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        return insert<custom_bc_strategy>(index, std::string_view {cstr, count});
    }

    // 4
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& insert (IndexLike auto index, const std::string& str) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        return insert<custom_bc_strategy>(index, std::string_view {str});
    }

    // 5
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& insert (IndexLike auto index, const std::string& str, size_type s_index, size_type count) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        return insert<custom_bc_strategy>(index, std::string_view {str}, s_index, count);
    }

    // 6
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr iterator insert (const_iterator pos, char ch) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        return insert<custom_bc_strategy>(pos, 1, ch);
    }

    // 7
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr iterator insert (const_iterator pos, size_type count, char ch) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        replace<custom_bc_strategy>(pos, pos, count, ch);
        return begin( ) + std::distance(cbegin( ), pos);
    }

    // 8
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr iterator insert (const_iterator pos, std::input_iterator auto first, std::input_iterator auto last) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        replace<custom_bc_strategy>(pos, pos, first, last);
        return begin( ) + std::distance(cbegin( ), pos);
    }

    // 9
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr iterator insert (const_iterator pos, std::initializer_list<char> ilist) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        return insert<custom_bc_strategy>(pos, ilist.begin( ), ilist.end( ));
    }

    // 10
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& insert (IndexLike auto index, const StringViewLike auto& sv) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        std::string_view t {sv};
        insert<custom_bc_strategy>(std::next(begin( ), index), t.cbegin( ), t.cend( ));
        return *this;
    }

    // 11
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& insert (IndexLike auto index, const StringViewLike auto& sv, size_type sv_index, size_type count = npos) noexcept(
        custom_bc_strategy != BoundCheckStrategy::Exception) {
        std::string_view t {sv};
        return insert<custom_bc_strategy>(index, t.substr(sv_index, count));
    }

    // 1
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& erase (IndexLike auto index = 0, size_type count = npos) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        return replace<custom_bc_strategy>(index, count == npos ? length( ) - index : count, "");
    }

    // 2
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr iterator erase (const_iterator position) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        replace<custom_bc_strategy>(position, std::next(position), "");
        return begin( ) + std::distance(cbegin( ), position);
    }

    // 3
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr iterator erase (const_iterator first, const_iterator last) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        replace<custom_bc_strategy>(first, last, "");
        return begin( ) + std::distance(cbegin( ), first);
    }

    // 1
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& replace (const IndexLike auto pos, size_type count, const std::string& str) {
        return replace<custom_bc_strategy>(pos, count, std::string_view {str});
    }

    // 2
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& replace (const_iterator first, const_iterator last, const std::string& str) {
        return replace<custom_bc_strategy>(first, last, str.cbegin( ), str.cend( ));
    }

    // 3
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& replace (IndexLike auto pos, size_type count, const std::string& str, IndexLike auto pos2, size_type count2 = npos) {
        return replace<custom_bc_strategy>(pos, count, std::string_view {str}, pos2, count2);
    }

    // 4
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& replace (IndexLike auto pos, size_type count, const char_type* cstr, size_type count2) {
        return replace<custom_bc_strategy>(pos, count, std::string_view {cstr, count2});
    }

    // 5
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& replace (const_iterator first, const_iterator last, const char_type* cstr, size_type count2) {
        return replace<custom_bc_strategy>(first, last, std::string_view {cstr, count2});
    }

    // 6
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& replace (IndexLike auto pos, size_type count, const char_type* cstr) {
        return replace<custom_bc_strategy>(pos, count, cstr, ::strlen(cstr));
    }

    // 7
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& replace (const_iterator first, const_iterator last, const char_type* cstr) {
        return replace<custom_bc_strategy>(first, last, std::string_view {cstr});
    }

    // 8
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& replace (IndexLike auto pos, size_type count, size_type count2, char_type ch) {
        return replace<custom_bc_strategy>(cbegin( ) + pos, cbegin( ) + pos + count, count2, ch);
    }

    // 9
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& replace (const_iterator first, const_iterator last, size_type count2, char_type ch) {
        auto count = std::distance(first, last);
        auto d     = count2 - count;
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Assert ) {
            assert(first >= cbegin( ));
            assert(last <= cend( ));
            assert(first <= last);
            assert(size( ) + d <= capacity( ));
        }
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Exception ) {
            if ( first < cbegin( ) )
                throw std::out_of_range("first < cbegin");
            if ( last > cend( ) )
                throw std::out_of_range("last > end");
            if ( first > last )
                throw std::range_error("first > last");
            if ( size( ) + d > capacity( ) )
                throw std::overflow_error("size+d > capacity");
        }
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::LimitToBound ) {
            if ( first > last ) {
                std::swap(first, last);
                count = -count;
            }
            if ( first > cend( ) && last > cend( ) ) {
                first = cend( );
                last  = cend( );
            } else if ( first < cbegin( ) && last < cbegin( ) ) {
                first = cbegin( );
                last  = cbegin( );
            }
            first  = std::max(first, cbegin( ));
            last   = std::min(last, cend( ));
            count2 = std::min(count2, free_space( ) - count);
            d      = count2 - count;
        }

        shift(last, d);
        auto f = begin( ) + std::distance(cbegin( ), first);
        std::fill_n(f, count2, ch);
        tail_ += d;
        *tail_ = '\0';
        return *this;
    }

    // 10
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& replace (const_iterator first, const_iterator last, std::input_iterator auto first2, std::input_iterator auto last2) {
        auto count  = std::distance(first, last);
        auto count2 = std::distance(first2, last2);
        auto d      = count2 - count;
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Assert ) {
            assert(first >= cbegin( ));
            assert(last <= cend( ));
            assert(first <= last);
            assert(size( ) + d <= capacity( ));
        }
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Exception ) {
            if ( first < cbegin( ) )
                throw std::out_of_range("first < cbegin");
            if ( last > cend( ) )
                throw std::out_of_range("last > end");
            if ( first > last )
                throw std::range_error("first > last");
            if ( size( ) + d > capacity( ) )
                throw std::overflow_error("size+d > capacity");
        }
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::LimitToBound ) {
            if ( first2 > last2 ) {
                std::swap(first2, last2);
                count2 = -count2;
            }
            if ( first > last ) {
                std::swap(first, last);
                count = -count;
            }
            if ( first > cend( ) && last > cend( ) ) {
                first = cend( );
                last  = cend( );
            } else if ( first < cbegin( ) && last < cbegin( ) ) {
                first = cbegin( );
                last  = cbegin( );
            }
            first  = std::max(first, cbegin( ));
            last   = std::min(last, cend( ));
            count2 = std::min(static_cast<size_type>(count2), free_space( ) - count);
            d      = count2 - count;
        }

        shift(last, d);
        auto f = begin( ) + std::distance(cbegin( ), first);
        std::copy_n(first2, count2, f);
        tail_ += d;
        *tail_ = '\0';
        return *this;
    }

    // 11
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& replace (const_iterator first, const_iterator last, std::initializer_list<char_type> ilist) {
        return replace<custom_bc_strategy>(first, last, ilist.begin( ), ilist.end( ));
    }

    // 12
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& replace (size_type pos, size_type count, const StringViewLike auto& sv) {
        return replace<custom_bc_strategy>(cbegin( ) + pos, cbegin( ) + pos + count, sv);
    }

    // 13
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& replace (const_iterator first, const_iterator last, const StringViewLike auto& sv) {
        std::string_view t {sv};
        return replace<custom_bc_strategy>(first, last, t.cbegin( ), t.cend( ));
    }

    // 14
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& replace (IndexLike auto pos, size_type count, const StringViewLike auto& sv, IndexLike auto pos2, size_type count2 = npos) {
        std::string_view t {sv};
        return replace<custom_bc_strategy>(pos, count, t.substr(pos2, count2));
    }

    ///@}

    ///@name iterators
    ///@{
    /**
     * @brief Returns an iterator pointing to the beginning of the string.
     *
     * This function returns a mutable iterator that points to the first character in the string.
     *
     * @return Iterator pointing to the start of the string.
     *
     * @see std::string::begin()
     *
     * @throw None. This method is noexcept.
     *
     * @par Example usage:
     * @code
     * char data_array[15] {"Hello, World!"};
     * wbr::static_string_adapter adapter{data_array, ::strlen(data_array)};
     * auto it = adapter.begin();
     * if (it != nullptr) {
     *     char first_char = *it; // first_char is 'H'
     * }
     * @endcode
     */
    [[nodiscard]] constexpr iterator begin ( ) noexcept {
        return head_;
    }

    /**
     * @brief Returns a const_iterator pointing to the beginning of the string.
     *
     * This function returns a constant iterator that points to the first character in the string.
     *
     * @return Const_iterator pointing to the start of the string.
     *
     * @see std::string::cbegin()
     *
     * @throw None. This method is noexcept.
     *
     * @par Example usage:
     * @code
     * char data_array[15] {"Hello, World!"};
     * const wbr::static_string_adapter adapter{data_array, ::strlen(data_array)};
     * auto it = adapter.cbegin();
     * if (it != nullptr) {
     *     char first_char = *it; // first_char is 'H'
     * }
     * @endcode
     */
    [[nodiscard]] constexpr const_iterator cbegin ( ) const noexcept {
        return head_;
    }

    /**
     * @brief Returns an iterator pointing to the beginning of the string (const version).
     *
     * This function returns a constant iterator that points to the first character in the string.
     *
     * @return Const_iterator pointing to the start of the string.
     *
     * @see std::string::begin() const
     *
     * @throw None. This method is noexcept.
     *
     * @par Example usage:
     * @code
     * char data_array[15] {"Hello, World!"};
     * const wbr::static_string_adapter adapter{data_array, ::strlen(data_array)};
     * auto it = adapter.begin();
     * if (it != nullptr) {
     *     char first_char = *it; // first_char is 'H'
     * }
     * @endcode
     */
    [[nodiscard]] constexpr const_iterator begin ( ) const noexcept {
        return cbegin( );
    }

    /**
     * @brief Returns an iterator pointing to the end of the string.
     *
     * This function returns a mutable iterator that points one past the last character in the string.
     *
     * @return Iterator pointing to one past the end of the string.
     *
     * @see std::string::end()
     *
     * @throw None. This method is noexcept.
     *
     * @par Example usage:
     * @code
     * char data_array[15] {"Hello, World!"};
     * wbr::static_string_adapter adapter{data_array, ::strlen(data_array)};
     * auto it = adapter.end();
     * if (it != nullptr) {
     *     char past_last_char = *(it - 1); // past_last_char is '!'
     * }
     * @endcode
     */
    [[nodiscard]] constexpr iterator end ( ) noexcept {
        return tail_;
    }

    /**
     * @brief Returns a const_iterator pointing to the end of the string.
     *
     * This function returns a constant iterator that points one past the last character in the string.
     *
     * @return Const_iterator pointing to one past the end of the string.
     *
     * @see std::string::cend()
     *
     * @throw None. This method is noexcept.
     *
     * @par Example usage:
     * @code
     * char data_array[15] {"Hello, World!"};
     * const wbr::static_string_adapter adapter{data_array, ::strlen(data_array)};
     * auto it = adapter.cend();
     * if (it != nullptr) {
     *     char past_last_char = *(it - 1); // past_last_char is '!'
     * }
     * @endcode
     */
    [[nodiscard]] constexpr const_iterator cend ( ) const noexcept {
        return tail_;
    }

    /**
     * @brief Returns an iterator pointing to the end of the string (const version).
     *
     * This function returns a constant iterator that points one past the last character in the string.
     *
     * @return Const_iterator pointing to one past the end of the string.
     *
     * @see std::string::end() const
     *
     * @throw None. This method is noexcept.
     *
     * @par Example usage:
     * @code
     * char data_array[15] {"Hello, World!"};
     * const wbr::static_string_adapter adapter{data_array, ::strlen(data_array)};
     * auto it = adapter.end();
     * if (it != nullptr) {
     *     char past_last_char = *(it - 1); // past_last_char is '!'
     * }
     * @endcode
     */
    [[nodiscard]] constexpr const_iterator end ( ) const noexcept {
        return cend( );
    }

    /**
     * @brief Returns a reverse_iterator pointing to the beginning of the reversed string.
     *
     * This function returns a mutable reverse iterator that points to the last character in the string.
     *
     * @return Reverse_iterator pointing to the start of the reversed string.
     *
     * @see std::string::rbegin()
     *
     * @throw None. This method is noexcept.
     *
     * @par Example usage:
     * @code
     * char data_array[15] {"Hello, World!"};
     * wbr::static_string_adapter adapter{data_array, ::strlen(data_array)};
     * auto rit = adapter.rbegin();
     * if (rit != nullptr) {
     *     char last_char = *rit; // last_char is '!'
     * }
     * @endcode
     */
    [[nodiscard]] constexpr reverse_iterator rbegin ( ) noexcept {
        return std::make_reverse_iterator(end( ));
    }

    /**
     * @brief Returns a const_reverse_iterator pointing to the beginning of the reversed string.
     *
     * This function returns a constant reverse iterator that points to the last character in the string.
     *
     * @return Const_reverse_iterator pointing to the start of the reversed string.
     *
     * @see std::string::crbegin()
     *
     * @throw None. This method is noexcept.
     *
     * @par Example usage:
     * @code
     * char data_array[15] {"Hello, World!"};
     * const wbr::static_string_adapter adapter{data_array, ::strlen(data_array)};
     * auto rit = adapter.crbegin();
     * if (rit != nullptr) {
     *     char last_char = *rit; // last_char is '!'
     * }
     * @endcode
     */
    [[nodiscard]] constexpr const_reverse_iterator crbegin ( ) const noexcept {
        return std::make_reverse_iterator(cend( ));
    }

    /**
     * @brief Returns a reverse_iterator pointing to the beginning of the reversed string (const version).
     *
     * This function returns a constant reverse iterator that points to the last character in the string.
     *
     * @return Const_reverse_iterator pointing to the start of the reversed string.
     *
     * @see std::string::rbegin() const
     *
     * @throw None. This method is noexcept.
     *
     * @par Example usage:
     * @code
     * char data_array[15] {"Hello, World!"};
     * const wbr::static_string_adapter adapter{data_array, ::strlen(data_array)};
     * auto rit = adapter.rbegin();
     * if (rit != nullptr) {
     *     char last_char = *rit; // last_char is '!'
     * }
     * @endcode
     */
    [[nodiscard]] constexpr const_reverse_iterator rbegin ( ) const noexcept {
        return crbegin( );
    }

    /**
     * @brief Returns a reverse_iterator pointing to the end of the reversed string.
     *
     * This function returns a mutable reverse iterator that points one before the first character in the string.
     *
     * @return Reverse_iterator pointing to one before the beginning of the reversed string.
     *
     * @see std::string::rend()
     *
     * @throw None. This method is noexcept.
     *
     * @par Example usage:
     * @code
     * char data_array[15] {"Hello, World!"};
     * wbr::static_string_adapter adapter{data_array, ::strlen(data_array)};
     * auto rit = adapter.rend();
     * if (rit != nullptr) {
     *     char first_char = *(rit - 1); // first_char is 'H'
     * }
     * @endcode
     */
    [[nodiscard]] constexpr reverse_iterator rend ( ) noexcept {
        return std::make_reverse_iterator(begin( ));
    }

    /**
     * @brief Returns a const_reverse_iterator pointing to the end of the reversed string.
     *
     * This function returns a constant reverse iterator that points one before the first character in the string.
     *
     * @return Const_reverse_iterator pointing to one before the beginning of the reversed string.
     *
     * @see std::string::crend()
     *
     * @throw None. This method is noexcept.
     *
     * @par Example usage:
     * @code
     * char data_array[15] {"Hello, World!"};
     * const wbr::static_string_adapter adapter{data_array, ::strlen(data_array)};
     * auto rit = adapter.crend();
     * if (rit != nullptr) {
     *     char first_char = *(rit - 1); // first_char is 'H'
     * }
     * @endcode
     */
    [[nodiscard]] constexpr const_reverse_iterator crend ( ) const noexcept {
        return std::make_reverse_iterator(cbegin( ));
    }

    /**
     * @brief Returns a reverse_iterator pointing to the end of the reversed string (const version).
     *
     * This function returns a constant reverse iterator that points one before the first character in the string.
     *
     * @return Const_reverse_iterator pointing to one before the beginning of the reversed string.
     *
     * @see std::string::rend() const
     *
     * @throw None. This method is noexcept.
     *
     * @par Example usage:
     * @code
     * char data_array[15] {"Hello, World!"};
     * const wbr::static_string_adapter adapter{data_array, ::strlen(data_array)};
     * auto rit = adapter.rend();
     * if (rit != nullptr) {
     *     char first_char = *(rit - 1); // first_char is 'H'
     * }
     * @endcode
     */
    [[nodiscard]] constexpr const_reverse_iterator rend ( ) const noexcept {
        return crend( );
    }

    ///@}

    ///@name operations
    ///@{

    // https://en.cppreference.com/w/cpp/string/basic_string/compare.html
    // 1
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr int compare (const std::string& str) const noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        return compare<custom_bc_strategy>(str.cbegin( ), str.cend( ));
    }

    // 2
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr int compare (IndexLike auto pos1, size_type count1, const std::string& str) const noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        std::string_view sv {str};
        return compare<custom_bc_strategy>(pos1, count1, sv);
    }

    // 3
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr int compare (IndexLike auto pos1, size_type count1, const std::string& str, IndexLike auto pos2, size_type count2 = npos) const
        noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        std::string_view sv {str};
        return compare<custom_bc_strategy>(pos1, count1, sv, pos2, count2);
    }

    // 4
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr int compare (const char_type* cstr) const noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        std::string_view sv {cstr};
        return compare<custom_bc_strategy>(sv);
    }

    // 5
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr int compare (IndexLike auto pos1, size_type count1, const char_type* s) const noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        std::string_view sv {s};
        return compare<custom_bc_strategy>(pos1, count1, sv);
    }

    // 6
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr int compare (IndexLike auto pos1, size_type count1, const char_type* s, size_type count2) const noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        std::string_view sv {s, count2};
        return compare<custom_bc_strategy>(pos1, count1, sv);
    }

    // 7
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr int compare (const StringViewLike auto& t) const noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        std::string_view sv {t};
        return compare<custom_bc_strategy>(sv.cbegin( ), sv.cend( ));
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr int compare (IndexLike auto pos1, size_type count1, const StringViewLike auto& t) const noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        std::string_view sv {t};
        return compare<custom_bc_strategy>(pos1, count1, sv.cbegin( ), sv.cend( ));
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr int compare (IndexLike auto pos1, size_type count1, const StringViewLike auto& t, IndexLike auto pos2, size_type count2 = npos) const
        noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        std::string_view sv {t};
        return compare<custom_bc_strategy>(pos1, count1, sv.substr(pos2, count2));
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr int compare (std::initializer_list<char_type> ilist) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        return compare<custom_bc_strategy>(ilist.begin( ), ilist.end( ));
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr int compare (const std::input_iterator auto& first, const std::input_iterator auto& last) const noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        return compare<custom_bc_strategy>(0, size( ), first, last);
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr int compare (IndexLike auto pos, size_type count, const std::input_iterator auto& first, const std::input_iterator auto& last) const
        noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Assert ) {
            assert(pos + count <= size( ));
            assert(first <= last);
        }
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Exception ) {
            if ( pos + count > size( ) )
                throw std::out_of_range("pos + count > size()");
            if ( first > last )
                throw std::range_error("first > last");
        }
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::LimitToBound ) {
            pos   = std::min(pos, size( ));
            count = std::min(count, size( ) - pos);
            if ( first > last )
                std::swap(first, last);
        }

        const auto c = std::lexicographical_compare_three_way(cbegin( ) + pos, cbegin( ) + pos + count, first, last);
        if ( c == std::strong_ordering::equal )
            return 0;
        if ( c == std::strong_ordering::less )
            return -1;
        return 1;
    }

    // https://en.cppreference.com/w/cpp/string/basic_string/starts_with.html
    // 1
    constexpr bool starts_with (const StringViewLike auto& sv) const noexcept {
        return view( ).starts_with(std::string_view {sv});
    }

    // 2
    constexpr bool starts_with (char ch) const noexcept {
        return view( ).starts_with(ch);
    }

    // 3
    constexpr bool starts_with (const char_type* cstr) const {
        return view( ).starts_with(cstr);
    }

    // https://en.cppreference.com/w/cpp/string/basic_string/ends_with.html
    // 1
    constexpr bool ends_with (const StringViewLike auto& sv) const noexcept {
        return view( ).starts_with(sv);
    }

    // 2
    constexpr bool ends_with (char_type ch) const noexcept {
        return view( ).starts_with(ch);
    }

    // 3
    constexpr bool ends_with (const char_type* cstr) const {
        return view( ).starts_with(cstr);
    }

    // https://en.cppreference.com/w/cpp/string/basic_string/contains.html
    // 1
    constexpr bool contains (const StringViewLike auto& sv) const noexcept {
        return find(sv) != npos;
    }

    // 2
    constexpr bool contains (char ch) const noexcept {
        return find(ch) != npos;
    }

    // 3
    constexpr bool contains (const char* cstr) const {
        return find(cstr) != npos;
    }

    // https://en.cppreference.com/w/cpp/string/basic_string/find.html
    // 1
    size_type find (const std::string& str, IndexLike auto pos = 0) const noexcept {
        return view( ).find(str, pos);
    }

    // 2
    size_type find (const char_type* cstr, IndexLike auto pos, size_type count) const noexcept {
        return view( ).find(cstr, pos, count);
    }

    // 3
    size_type find (const char_type* cstr, IndexLike auto pos) const noexcept {
        return view( ).find(cstr, pos);
    }

    size_type find (const char_type* cstr) const noexcept {
        return view( ).find(cstr, 0);
    }

    // 4
    size_type find (char_type ch, IndexLike auto pos) const noexcept {
        return view( ).find(ch, pos);
    }

    size_type find (char_type ch) const noexcept {
        return find(ch, 0);
    }

    // 5
    size_type find (const StringViewLike auto& t, IndexLike auto pos = 0) const noexcept {
        return view( ).find(std::string_view {t}, pos);
    }

    ///@}

    constexpr char at (IndexLike auto pos) const {
        if constexpr ( std::is_signed_v<decltype(pos)> )
            if ( pos < 0 )
                throw std::out_of_range("pos < 0");
        if ( static_cast<size_type>(pos) >= size( ) )
            throw std::out_of_range("pos >= size()");
        return *(head_ + static_cast<size_type>(pos));
    }

    constexpr char& at (IndexLike auto pos) {
        if constexpr ( std::is_signed_v<decltype(pos)> )
            if ( pos < 0 )
                throw std::out_of_range("pos < 0");
        if ( static_cast<size_type>(pos) >= size( ) )
            throw std::out_of_range("pos >= size()");
        return *(head_ + static_cast<size_type>(pos));
    }

    [[nodiscard]] constexpr explicit operator std::string ( ) const noexcept(noexcept(view( ))) {
        return std::string {view( )};
    }

    constexpr void reserve ( ) noexcept {
        // for compatibility only, do nothing since static_string don't reallocate memory
        return;
    }

    constexpr void shrink_to_fit ( ) noexcept {
        // for compatibility only, do nothing since static_string don't reallocate memory
        return;
    }

    [[nodiscard]] constexpr std::string_view substr (IndexLike auto pos = 0, size_type count = npos) const {
        return view( ).substr(pos, count);
    }

    constexpr size_type copy (char* dest, size_type count, IndexLike auto pos = 0) const {
        return view( ).copy(dest, count, pos);
    }

    /*
     * public:
    constexpr size_type copy(char* dest, size_type count, size_type pos = 0) const;
    constexpr void resize(size_type count);
    constexpr void resize(size_type count, char ch);
    constexpr void swap(string& other) noexcept;
    constexpr size_type find_first_of(const string& str, size_type pos = 0) const noexcept;
    constexpr size_type find_first_of(const std::string& str, size_type pos = 0) const noexcept;
    constexpr size_type find_first_of(const char* s, size_type pos, size_type count) const;
    constexpr size_type find_first_of(const char* s, size_type pos = 0) const;
    constexpr size_type find_first_of(char ch, size_type pos = 0) const noexcept;
    constexpr size_type find_first_of(std::string_view sv, size_type startPos = 0) const noexcept;
    constexpr size_type find_first_not_of(const string& str, size_type pos = 0) const noexcept;
    constexpr size_type find_first_not_of(const std::string& str, size_type pos = 0) const noexcept;
    constexpr size_type find_first_not_of(const char* s, size_type pos, size_type count) const;
    constexpr size_type find_first_not_of(const char* s, size_type pos = 0) const;
    constexpr size_type find_first_not_of(char ch, size_type pos = 0) const noexcept;
    constexpr size_type find_first_not_of(std::string_view sv, size_type startPos = 0) const noexcept;
    constexpr size_type find_last_of(const string& str, size_type pos = 0) const noexcept;
    constexpr size_type find_last_of(const std::string& str, size_type pos = 0) const noexcept;
    constexpr size_type find_last_of(const char* s, size_type pos, size_type count) const;
    constexpr size_type find_last_of(const char* s, size_type pos = 0) const;
    constexpr size_type find_last_of(char ch, size_type pos = npos) const noexcept;
    constexpr size_type find_last_of(std::string_view sv, size_type startPos = 0) const noexcept;
    constexpr size_type find_last_not_of(const string& str, size_type pos = 0) const noexcept;
    constexpr size_type find_last_not_of(const std::string& str, size_type pos = 0) const noexcept;
    constexpr size_type find_last_not_of(const char* s, size_type pos, size_type count) const;
    constexpr size_type find_last_not_of(const char* s, size_type pos = 0) const;
    constexpr size_type find_last_not_of(char ch, size_type pos = npos) const noexcept;
    constexpr size_type find_last_not_of(std::string_view sv, size_type startPos = 0) const noexcept;
    constexpr bool operator== (const skl::string& mystr) const;
    constexpr bool operator== (std::string_view sv) const;
    constexpr bool operator== (const std::string& str) const;
    constexpr bool operator== (const char* cstr) const;
    constexpr int operator<=> (const skl::string& mystr) const;
    constexpr int operator<=> (const std::string& str) const;
    constexpr int operator<=> (std::string_view sv) const;
    constexpr int operator<=> (const char* cstr) const;

    template<typename... T>
    constexpr skl::string& formatAppend (fmt::format_string<T...> fmt, T&&... args)
    template<typename... T>
    constexpr skl::string& formatAssign (fmt::format_string<T...> fmt, T&&... args)
     */
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

    constexpr void shift_left (size_t index, size_t n) {
        const auto p = begin( ) + index;
        std::copy(p, end( ), p - n);
    }

    constexpr void shift_right (size_t index, size_t n) {
        const auto p = begin( ) + index;
        std::copy_backward(p, end( ), std::next(end( ), n));
    }

    constexpr void shift_left (const_iterator p, size_t n) {
        return shift_left(std::distance(cbegin( ), p), n);
    }

    constexpr void shift_right (const_iterator p, size_t n) {
        return shift_right(std::distance(cbegin( ), p), n);
    }

    constexpr void shift (const_iterator pos, ssize_t n) {
        if ( n > 0 )
            shift_right(pos, n);
        else
            shift_left(pos, -n);
    }
};

template<std::size_t SZ, BoundCheckStrategy bc_strategy = BoundCheckStrategy::NoCheck>
class static_string :
    public static_string_adapter<bc_strategy>

{
    using parent = static_string_adapter<bc_strategy>;

public:
    static_string ( ) : parent {data_.data( ), SZ, 0} {
    }

    static_string (std::string_view sv) : static_string( ) {
        parent::assign(sv);
    }

private:
    std::array<typename parent::char_type, SZ> data_;
};

}  // namespace wbr
