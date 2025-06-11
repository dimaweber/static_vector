#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <stdexcept>

#include "bound_check.hxx"

#if BUILD_TESTS
    #include <gtest/gtest.h>
#endif

namespace wbr {

template<typename SV>
concept StringViewLike = std::is_convertible_v<const SV&, std::string_view> && !std::is_convertible_v<const SV&, const char*>;

template<typename I>
concept IndexLike = std::is_integral_v<I>;

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

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& append (std::input_iterator auto first, std::input_iterator auto last) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        insert<custom_bc_strategy>(end( ), first, last);
        return *this;
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& append (size_type count, char ch) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        insert<custom_bc_strategy>(cend( ), count, ch);
        return *this;
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& append (char c) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        return append<custom_bc_strategy>(1, c);
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& append (const char_type* s, size_type count) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        return append<custom_bc_strategy>(std::string_view {s, count});
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& append (const char_type* s) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        return append<custom_bc_strategy>(s, ::strlen(s));
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& append (const StringViewLike auto& t) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        const std::string_view sv = t;
        return append<custom_bc_strategy>(sv.cbegin( ), sv.cend( ));
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& append (const StringViewLike auto& t, IndexLike auto pos, size_type count = npos) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        const std::string_view sv = t;
        return append<custom_bc_strategy>(sv.substr(pos, count));
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& append (std::initializer_list<char> ilist) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        return append<custom_bc_strategy>(ilist.begin( ), ilist.end( ));
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& append (const std::string& str) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        return append<custom_bc_strategy>(str.data( ), str.size( ));
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& append (const std::string& str, IndexLike auto pos, size_type count = npos) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        const std::string_view sv {str};
        return append<custom_bc_strategy>(sv.substr(pos, count));
    }

    /// @see https://en.cppreference.com/w/cpp/string/basic_string/assign
    // 1
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& assign(const std::string& str);

    // 2
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& assign(std::string&& str) noexcept;

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
    constexpr static_string_adapter& assign(const std::string& str, IndexLike auto pos, size_type count = npos);

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

    // 2
    constexpr static_string_adapter& operator= (std::string&& sv) noexcept;

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
    static_string_adapter& operator= (nullptr_t) = delete;

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
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Exception ) {
            if ( pos < cbegin( ) )
                throw std::out_of_range("pos less then begin()");
            if ( pos > cend( ) )
                throw std::out_of_range("pos greater then end()");
            if ( size( ) + count > max_size( ) )
                throw std::overflow_error("size() + count exceeds max_size()");
        }
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Assert ) {
            assert(pos >= cbegin( ));
            assert(pos <= cend( ));
            assert(size( ) + count <= max_size( ));
        }
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::LimitToBound ) {
            pos   = std::max(pos, cbegin( ));
            pos   = std::min(pos, cend( ));
            count = std::min(count, max_size( ) - size( ));
        }

        size_type index = pos - cbegin( );
        insertHelper(index, count);
        std::fill_n(begin( ) + index, count, ch);
        return begin( ) + index;
    }

    // 8
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr iterator insert (const_iterator pos, std::input_iterator auto first, std::input_iterator auto last) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        size_type count = std::distance(first, last);
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Exception ) {
            if ( pos < cbegin( ) )
                throw std::out_of_range("pos less then begin()");
            if ( pos > cend( ) )
                throw std::out_of_range("pos greater then end()");
            if ( size( ) + count > max_size( ) )
                throw std::overflow_error("size() + count exceeds max_size()");
            if ( first > last )
                throw std::range_error("first it past last");
        }
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Assert ) {
            assert(pos >= cbegin( ));
            assert(pos <= cend( ));
            assert(size( ) + count <= max_size( ));
            assert(first <= last);
        }
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::LimitToBound ) {
            if ( first > last )
                count = std::distance(last, first);
            pos   = std::max(pos, cbegin( ));
            pos   = std::min(pos, cend( ));
            count = std::min(count, max_size( ) - size( ));
        }

        const size_type index = std::distance(cbegin( ), pos);
        insertHelper(index, count);
        std::copy_n(first, count, begin( ) + index);
        return begin( ) + index;
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
        insert<custom_bc_strategy>(index, t.substr(sv_index, count));
    }

    // 1
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr static_string_adapter& erase (IndexLike auto index = 0, size_type count = npos) {
        if ( index > length( ) )
            throw std::out_of_range("index out of range");
        if ( count == npos )
            count = length( ) - index;
        else
            count = std::min(length( ) - index, count);
        tail_  = std::copy(std::next(begin( ), index + count), end( ), begin( ) + index);
        *tail_ = '\0';
        return *this;
    }

    // 2
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr iterator erase (const_iterator position) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Exception ) {
            if ( position < cbegin( ) )
                throw std::out_of_range("position is lesser then begin()");
            if ( position <= cend( ) )
                throw std::out_of_range("position it greater then end()");
        }
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Assert ) {
            assert(position >= cbegin( ));
            assert(position < cend( ));
            assert(!empty( ));
        }
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::LimitToBound ) {
            position = std::max(position, cbegin( ));
            position = std::min(position, cend( ));
        }

        const auto idx = std::distance(begin( ), position);
        const auto pos = begin( ) + idx;
        tail_          = std::copy(std::next(pos), end( ), pos);
        *tail_         = '\0';
        return std::next(pos);
    }

    // 3
    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr iterator erase (const_iterator first, const_iterator last) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        iterator f = begin( ) + std::distance(begin( ), first);
        iterator l = begin( ) + std::distance(begin( ), last);
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Exception ) {
            if ( first < cbegin( ) )
                throw std::out_of_range("first is lesser then begin()");
            if ( last > cend( ) )
                throw std::out_of_range("las is greater then end()");
            if ( first > last )
                throw std::range_error("first is greater then last");
        }
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::Assert ) {
            assert(first >= cbegin( ));
            assert(last <= cend( ));
            assert(first <= last);
        }
        if constexpr ( custom_bc_strategy == BoundCheckStrategy::LimitToBound ) {
            if ( f > l )
                std::swap(f, l);
            f = std::max(f, begin( ));
            l = std::min(l, end( ));
        }

        tail_  = std::copy(l, end( ), f);
        *tail_ = '\0';
        return std::next(f);
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

    /*
     * public:
    constexpr string& operator= (std::string_view sv);
    constexpr string& operator= (const std::string& str);
    constexpr string& operator= (const char* cstr);
    constexpr string& operator= (const string& mystr);
    constexpr string& assign(std::input_iterator auto first, std::input_iterator auto last);
    constexpr string& assign(size_type count, char c);
    constexpr string& assign(std::string_view sv);
    constexpr string& assign(std::string_view sv, size_type pos, size_type count = npos);
    constexpr string& assign(const char* str);
    constexpr string& assign(const char* str, size_type count);
    constexpr string& assign(std::initializer_list<char> ilist);
    constexpr char  at(size_type pos) const;
    constexpr char& at(size_type pos);
    constexpr char  operator[] (size_type pos) const;
    constexpr char& operator[] (size_type pos);
    constexpr const char& front( ) const;
    constexpr char&       front( );
    constexpr const char& back( ) const;
    constexpr char&       back( );
    constexpr virtual const char* data( ) const noexcept = 0;
    constexpr virtual char*       data( ) noexcept       = 0;
    constexpr const char* c_str( ) const noexcept;
    constexpr std::string_view view( ) const noexcept;
    constexpr                  operator std::string_view ( ) const noexcept;
    constexpr std::string std_str( ) const noexcept;
    constexpr iterator               begin( ) noexcept;
    constexpr const_iterator         begin( ) const noexcept;
    constexpr iterator               end( ) noexcept;
    constexpr const_iterator         end( ) const noexcept;
    constexpr const_iterator         cbegin( ) const;
    constexpr const_iterator         cend( ) const;
    constexpr reverse_iterator       rbegin( );
    constexpr reverse_iterator       rend( );
    constexpr const_reverse_iterator crbegin( ) const;
    constexpr const_reverse_iterator crend( ) const;
    constexpr bool empty( ) const noexcept;
    constexpr virtual size_type length( ) const noexcept;
    constexpr size_type         size( ) const noexcept;
    constexpr virtual size_type max_size( ) const noexcept = 0;
    constexpr virtual size_type capacity( ) const noexcept = 0;
    constexpr size_type free_space( ) const noexcept;
    constexpr virtual void reserve(size_type new_cap) = 0;
    virtual void shrink_to_fit( ) = 0;
    constexpr void clear( ) noexcept;
    constexpr iterator insert(const_iterator pos, std::input_iterator auto first, std::input_iterator auto last);
    constexpr iterator insert(const_iterator pos, char ch);
    constexpr iterator insert(const_iterator pos, size_type count, char ch);
    constexpr string&  insert(size_type index, size_type count, char ch);
    constexpr iterator insert(const_iterator pos, std::initializer_list<char> ilist);
    constexpr string&  insert(size_type index, std::string_view sv);
    constexpr string&  insert(size_type index, std::string_view sv, size_type index_str, size_type count = npos);
    constexpr string&  insert(size_type index, const char* cstr);
    constexpr string&  insert(size_type index, const char* cstr, size_type count);
    constexpr string&  insert(size_type index, const std::string& str);
    constexpr string&  insert(size_type index, const std::string& str, size_type index_str, size_type count = npos);
    constexpr string&  insert(size_type index, const skl::string& mystr);
    constexpr string&  insert(size_type index, const skl::string& mystr, size_type index_str, size_type count = npos);
    constexpr string&  erase(size_type index = 0, size_type count = npos);
    constexpr iterator erase(const_iterator position);
    constexpr iterator erase(const_iterator first, const_iterator last);
    constexpr void push_back(char c);
    constexpr void pop_back( );
    constexpr string& append(std::input_iterator auto  first, std::input_iterator auto last);
    constexpr string& append(size_type count, char ch);
    constexpr string& append(char ch);
    constexpr string& append(const std::string& str);
    constexpr string& append(const string& mystr);
    constexpr string& append(const char* cstr);
    constexpr string& append(std::string_view sv);
    constexpr string& append(const std::string& str, size_type pos, size_type count);
    constexpr string& append(const string& mystr, size_type pos, size_type count);
    constexpr string& append(std::string_view sv, size_type pos, size_type count);
    constexpr string& append(std::string_view sv, size_type count);
    constexpr string& append(std::initializer_list<char> ilist);
    constexpr string& operator+= (const string& str);
    constexpr string& operator+= (const std::string& str);
    constexpr string& operator+= (std::string_view str);
    constexpr string& operator+= (const char* str);
    constexpr string& operator+= (char);
    constexpr string& operator+= (std::initializer_list<char> ilist);
    constexpr int compare(const string& secondR) const;
    constexpr int compare(size_type pos1, size_type count1, const string& str) const;
    constexpr int compare(size_type pos1, size_type count1, const string& str, size_type pos2, size_type count2 = npos) const;
    constexpr int compare(const std::string& str) const noexcept;
    constexpr int compare(size_type pos1, size_type count1, const std::string& str) const;
    constexpr int compare(size_type pos1, size_type count1, const std::string& str, size_type pos2, size_type count2 = npos) const;
    constexpr int compare(const char* s) const;
    constexpr int compare(size_type pos1, size_type count1, const char* s) const;
    constexpr int compare(size_type pos1, size_type count1, const char* s, size_type count2) const;
    constexpr int compare(std::string_view t) const noexcept;
    constexpr int compare(size_type pos1, size_type count1, std::string_view t) const;
    constexpr int compare(size_type pos1, size_type count1, std::string_view t, size_type pos2, size_type count2 = npos) const;
    constexpr bool starts_with(std::string_view secondP) const noexcept;
    constexpr bool starts_with(char ch) const noexcept;
    constexpr bool starts_with(const char*) const;
    constexpr bool ends_with(std::string_view sv) const noexcept;
    constexpr bool ends_with(char ch) const noexcept;
    constexpr bool ends_with(const char* s) const;
    constexpr bool contains(std::string_view sv) const noexcept;
    constexpr bool contains(char ch) const noexcept;
    constexpr bool contains(const char* s) const;
    constexpr string& replace(const_iterator first, const_iterator last, std::input_iterator auto first2, std::input_iterator auto last2);
    constexpr string& replace(size_type pos, size_type count, const std::string& str);
    constexpr string& replace(const_iterator first, const_iterator last, const std::string& str);
    constexpr string& replace(size_type pos, size_type count, const std::string& str, size_type pos2, size_type count2 = npos);
    constexpr string& replace(size_type pos, size_type count, const char* cstr, size_type count2);
    constexpr string& replace(const_iterator first, const_iterator last, const char* cstr, size_type count2);
    constexpr string& replace(size_type pos, size_type count, const char* cstr);
    constexpr string& replace(const_iterator first, const_iterator last, const char* cstr);
    constexpr string& replace(size_type pos, size_type count, size_type count2, char ch);
    constexpr string& replace(const_iterator first, const_iterator last, size_type count2, char ch);
    constexpr string& replace(const_iterator first, const_iterator last, std::initializer_list<char> ilist);
    constexpr string& replace(size_type pos, size_type count, std::string_view sv);
    constexpr string& replace(const_iterator first, const_iterator last, std::string_view t);
    constexpr string& replace(size_type pos, size_type count, std::string_view t, size_type pos2, size_type count2 = npos);
    constexpr std::string_view substr(size_type pos = 0, size_type count = npos) const;
    constexpr size_type copy(char* dest, size_type count, size_type pos = 0) const;
    constexpr void resize(size_type count);
    constexpr void resize(size_type count, char ch);
    constexpr void swap(string& other) noexcept;
    constexpr size_type find(const std::string& str, size_type pos = 0) const noexcept;
    constexpr size_type find(const char* s, size_type pos, size_type count) const;
    constexpr size_type find(const char* pattern, size_type startPos = 0) const;
    constexpr size_type find(char c, size_type startPos = 0) const noexcept;
    constexpr size_type find(std::string_view t, size_type pos = 0) const noexcept;
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

    constexpr void insertHelper (size_t index, size_t count) {
        const size_t tailLen = length( ) - index;
        tail_ += std::min(free_space( ), count);
        std::copy_backward(begin( ) + index, begin( ) + index + tailLen, tail_);
        *tail_ = '\0';
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

private:
    std::array<typename parent::char_type, SZ> data_;
};

}  // namespace wbr
