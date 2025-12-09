#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <span>
#include <stdexcept>
#include <string_view>

#include "bound_check.hxx"
#include "concepts.hxx"

#if FMT_SUPPORT
    #include <fmt/format.h>
    #if __has_include(<fmt/ranges.h>)
        #include <fmt/ranges.h>
        #ifndef WBR_FMT_RANGES_INCLUDED
            #define WBR_FMT_RANGES_INCLUDED
        #endif
    #endif
#endif
#if STD_FORMAT_SUPPORT
    #include <format>
#endif

namespace wbr {
// Forward declaration for fmt formatter specialization
template<std::unsigned_integral LenType, BoundCheckStrategy bc_strategy>
class len_string_adapter;
}  // namespace wbr

#if FMT_SUPPORT

FMT_BEGIN_NAMESPACE

// Mark len_string_adapter as string-like for fmt
template<typename LenType, BoundCheckStrategy bc>
struct range_format_kind<wbr::len_string_adapter<LenType, bc>, char, void> {
    static constexpr auto value = range_format::string;
};

FMT_END_NAMESPACE
#endif

#if STD_FORMAT_SUPPORT
// std::format formatter for len_string_adapter
template<typename LenType, BoundCheckStrategy bc>
struct std::formatter<wbr::len_string_adapter<LenType, bc>, char> : std::formatter<std::string_view, char> {
    template<typename FormatContext>
    auto format (const wbr::len_string_adapter<LenType, bc>& str, FormatContext& ctx) const {
        return std::formatter<std::string_view, char>::format(str.view( ), ctx);
    }
};
#endif

namespace wbr {
/**
 * @class len_string_adapter
 *
 * @brief An adapter class for managing buffer+length style strings commonly used in embedded systems.
 *
 * This class provides a std::string-like interface for character buffers that store their length
 * in a separate variable rather than using null-termination. This is common in communication
 * protocols and embedded systems where structures contain fixed-size buffers with length fields.
 *
 * @tparam LenType The type used to store the length (e.g., uint8_t, uint16_t, size_t)
 * @tparam bc_strategy The bound-checking strategy to use
 *
 * @par Example Protocol Structure
 * @code
 * struct ChatMessage {
 *     uint64_t sender_id;
 *     uint64_t receiver_id;
 *     char message[256];
 *     uint8_t len;
 * };
 *
 * ChatMessage msg;
 * wbr::len_string_adapter<uint8_t> adapter(msg.message, 256, msg.len);
 * adapter.assign("Hello, World!");
 * // msg.len is now 13, message contains "Hello, World!"
 * @endcode
 */
template<std::unsigned_integral LenType = std::size_t, BoundCheckStrategy bc_strategy = BoundCheckStrategy::NoCheck>
class len_string_adapter {
public:
    /// @name nested types
    ///@{
    using char_type     = char;
    using pointer       = char_type*;
    using const_pointer = const char_type*;
    using size_type     = std::size_t;
    using len_type      = LenType;
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

    static_assert(std::is_integral_v<LenType>, "LenType must be an integral type");
    static_assert(std::is_unsigned_v<LenType>, "LenType should be unsigned");
    static_assert(std::contiguous_iterator<iterator>);
    ///@}

    static constexpr size_type npos = static_cast<size_type>(-1);

    /// @name constructors
    /// @{

    /**
     * @brief Constructs a len_string_adapter from a buffer pointer, capacity, and length reference.
     *
     * @param buffer Pointer to the character buffer
     * @param buffer_capacity Maximum capacity of the buffer
     * @param length Reference to the length variable (will be maintained by the adapter)
     */
    constexpr len_string_adapter (char_type* buffer, size_type buffer_capacity, len_type& length) : buffer_ {buffer}, capacity_ {buffer_capacity}, length_ {length} {
        using enum BoundCheckStrategy;
        if constexpr ( bc_strategy == Assert ) {
            assert(buffer != nullptr);
            assert(buffer_capacity > 0);
            assert(length <= buffer_capacity);
            assert(length <= std::numeric_limits<LenType>::max( ));
        }
        if constexpr ( bc_strategy == Exception ) {
            if ( buffer == nullptr )
                throw std::runtime_error {"null pointer"};
            if ( buffer_capacity == 0 )
                throw std::length_error {"zero capacity"};
            if ( length > buffer_capacity )
                throw std::length_error {"initial length exceeds capacity"};
        }
        if constexpr ( bc_strategy == LimitToBound ) {
            if ( length > buffer_capacity )
                length_ = static_cast<len_type>(buffer_capacity);
        }
    }

    /**
     * @brief Constructs from a std::array and length reference.
     */
    template<size_type N>
    constexpr len_string_adapter(std::array<char_type, N>& arr, len_type& length) : len_string_adapter(arr.data( ), N, length) {
    }

    /**
     * @brief Constructs from a C-style array and length reference.
     */
    template<size_type N>
    constexpr len_string_adapter(char_type (&arr)[N], len_type& length) : len_string_adapter(arr, N, length) {
    }

    /**
     * @brief Constructs from two iterators and length reference.
     */
    template<std::contiguous_iterator Iter>
    constexpr len_string_adapter(Iter first, Iter last, len_type& length) : len_string_adapter(std::to_address(first), std::distance(first, last), length) {
    }

    /**
     * @brief Constructs from a std::span and length reference.
     */
    template<size_type Extent = std::dynamic_extent>
    constexpr len_string_adapter(std::span<char_type, Extent> sp, len_type& length) : len_string_adapter(sp.data( ), sp.size( ), length) {
    }

    /// @}

    ///@name capacity
    ///@{

    [[nodiscard]] constexpr size_type size ( ) const noexcept {
        return static_cast<size_type>(length_);
    }

    [[nodiscard]] constexpr size_type length ( ) const noexcept {
        return size( );
    }

    [[nodiscard]] constexpr size_type max_size ( ) const noexcept {
        return std::min(capacity_, static_cast<size_type>(std::numeric_limits<LenType>::max( )));
    }

    [[nodiscard]] constexpr size_type capacity ( ) const noexcept {
        return max_size( );
    }

    [[nodiscard]] constexpr bool empty ( ) const noexcept {
        return length_ == 0;
    }

    [[nodiscard]] constexpr size_type free_space ( ) const noexcept {
        return max_size( ) - size( );
    }

    ///@}

    /// @name element access
    /// @{

    [[nodiscard]] constexpr reference operator[] (IndexLike auto pos) noexcept {
        return buffer_[pos];
    }

    [[nodiscard]] constexpr const_reference operator[] (IndexLike auto pos) const noexcept {
        return buffer_[pos];
    }

    [[nodiscard]] constexpr reference at (IndexLike auto pos) {
        if constexpr ( std::is_signed_v<decltype(pos)> )
            if ( pos < 0 )
                throw std::out_of_range("pos < 0");
        if ( static_cast<size_type>(pos) >= size( ) )
            throw std::out_of_range("pos >= size()");
        return buffer_[pos];
    }

    [[nodiscard]] constexpr const_reference at (IndexLike auto pos) const {
        if constexpr ( std::is_signed_v<decltype(pos)> )
            if ( pos < 0 )
                throw std::out_of_range("pos < 0");
        if ( static_cast<size_type>(pos) >= size( ) )
            throw std::out_of_range("pos >= size()");
        return buffer_[pos];
    }

    [[nodiscard]] constexpr reference front ( ) {
        return *begin( );
    }

    [[nodiscard]] constexpr const_reference front ( ) const {
        return *begin( );
    }

    [[nodiscard]] constexpr reference back ( ) {
        return *(end( ) - 1);
    }

    [[nodiscard]] constexpr const_reference back ( ) const {
        return *(end( ) - 1);
    }

    [[nodiscard]] constexpr pointer data ( ) noexcept {
        return buffer_;
    }

    [[nodiscard]] constexpr const_pointer data ( ) const noexcept {
        return buffer_;
    }

    [[nodiscard]] constexpr std::string_view view ( ) const noexcept {
        return std::string_view {buffer_, size( )};
    }

    [[nodiscard]] constexpr operator std::string_view ( ) const noexcept {
        return view( );
    }

    /**
     * @brief Returns a null-terminated C-string if possible.
     *
     * @note This will add null-termination if there's space (size < capacity).
     *       If there's no space, returns nullptr or throws based on strategy.
     *
     * @return Pointer to null-terminated string, or nullptr if impossible
     */
    [[nodiscard]] constexpr const_pointer c_str ( ) const noexcept(bc_strategy != BoundCheckStrategy::Exception) {
        if ( size( ) < capacity_ ) {
            const_cast<char_type*>(buffer_)[size( )] = '\0';
            return buffer_;
        }

        using enum BoundCheckStrategy;
        if constexpr ( bc_strategy == Exception ) {
            throw std::length_error("no space for null terminator");
        }
        return nullptr;
    }

    ///@}

    ///@name modifiers
    ///@{

    constexpr void clear ( ) noexcept {
        length_ = 0;
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr len_string_adapter& assign (std::string_view sv) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        clear( );
        return append<custom_bc_strategy>(sv);
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr len_string_adapter& assign (const char* s) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        return assign<custom_bc_strategy>(std::string_view {s});
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr len_string_adapter& assign (size_type count, char ch) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        clear( );
        return append<custom_bc_strategy>(count, ch);
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr len_string_adapter& append (std::string_view sv) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        auto append_size = sv.size( );

        using enum BoundCheckStrategy;
        if constexpr ( custom_bc_strategy == Assert ) {
            assert(size( ) + append_size <= max_size( ));
        }
        if constexpr ( custom_bc_strategy == Exception ) {
            if ( size( ) + append_size > max_size( ) )
                throw std::overflow_error("append would exceed capacity");
        }
        if constexpr ( custom_bc_strategy == LimitToBound ) {
            append_size = std::min(append_size, free_space( ));
        }

        std::copy_n(sv.data( ), append_size, buffer_ + size( ));
        length_ = static_cast<len_type>(size( ) + append_size);
        return *this;
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr len_string_adapter& append (size_type count, char ch) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        using enum BoundCheckStrategy;
        if constexpr ( custom_bc_strategy == Assert ) {
            assert(size( ) + count <= max_size( ));
        }
        if constexpr ( custom_bc_strategy == Exception ) {
            if ( size( ) + count > max_size( ) )
                throw std::overflow_error("append would exceed capacity");
        }
        if constexpr ( custom_bc_strategy == LimitToBound ) {
            count = std::min(count, free_space( ));
        }

        std::fill_n(buffer_ + size( ), count, ch);
        length_ = static_cast<len_type>(size( ) + count);
        return *this;
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr len_string_adapter& append (const char* s) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        return append<custom_bc_strategy>(std::string_view {s});
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr len_string_adapter& operator+= (std::string_view sv) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        return append<custom_bc_strategy>(sv);
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr len_string_adapter& operator+= (const char* s) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        return append<custom_bc_strategy>(s);
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr len_string_adapter& operator+= (char ch) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        return append<custom_bc_strategy>(1, ch);
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr void push_back (char ch) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        append<custom_bc_strategy>(1, ch);
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr void pop_back ( ) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        using enum BoundCheckStrategy;
        if constexpr ( custom_bc_strategy == Assert ) {
            assert(!empty( ));
        }
        if constexpr ( custom_bc_strategy == Exception ) {
            if ( empty( ) )
                throw std::length_error("pop_back on empty string");
        }
        if ( !empty( ) ) {
            --length_;
        }
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr void resize (size_type count) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        resize<custom_bc_strategy>(count, '\0');
    }

    template<BoundCheckStrategy custom_bc_strategy = bc_strategy>
    constexpr void resize (size_type count, char ch) noexcept(custom_bc_strategy != BoundCheckStrategy::Exception) {
        using enum BoundCheckStrategy;
        if constexpr ( custom_bc_strategy == Assert ) {
            assert(count <= max_size( ));
        }
        if constexpr ( custom_bc_strategy == Exception ) {
            if ( count > max_size( ) )
                throw std::length_error("resize count exceeds capacity");
        }
        if constexpr ( custom_bc_strategy == LimitToBound ) {
            count = std::min(count, max_size( ));
        }

        if ( count > size( ) ) {
            std::fill_n(buffer_ + size( ), count - size( ), ch);
        }
        length_ = static_cast<len_type>(count);
    }

    ///@}

    ///@name iterators
    ///@{

    [[nodiscard]] constexpr iterator begin ( ) noexcept {
        return buffer_;
    }

    [[nodiscard]] constexpr const_iterator begin ( ) const noexcept {
        return buffer_;
    }

    [[nodiscard]] constexpr const_iterator cbegin ( ) const noexcept {
        return buffer_;
    }

    [[nodiscard]] constexpr iterator end ( ) noexcept {
        return buffer_ + size( );
    }

    [[nodiscard]] constexpr const_iterator end ( ) const noexcept {
        return buffer_ + size( );
    }

    [[nodiscard]] constexpr const_iterator cend ( ) const noexcept {
        return buffer_ + size( );
    }

    [[nodiscard]] constexpr reverse_iterator rbegin ( ) noexcept {
        return std::make_reverse_iterator(end( ));
    }

    [[nodiscard]] constexpr const_reverse_iterator rbegin ( ) const noexcept {
        return std::make_reverse_iterator(end( ));
    }

    [[nodiscard]] constexpr const_reverse_iterator crbegin ( ) const noexcept {
        return std::make_reverse_iterator(cend( ));
    }

    [[nodiscard]] constexpr reverse_iterator rend ( ) noexcept {
        return std::make_reverse_iterator(begin( ));
    }

    [[nodiscard]] constexpr const_reverse_iterator rend ( ) const noexcept {
        return std::make_reverse_iterator(begin( ));
    }

    [[nodiscard]] constexpr const_reverse_iterator crend ( ) const noexcept {
        return std::make_reverse_iterator(cbegin( ));
    }

    ///@}

    ///@name comparison operators
    ///@{

    constexpr bool operator== (std::string_view sv) const noexcept {
        return view( ) == sv;
    }

    constexpr bool operator== (const char* s) const noexcept {
        return view( ) == s;
    }

    constexpr int operator<=> (std::string_view sv) const noexcept {
        auto cmp = view( ).compare(sv);
        return cmp < 0 ? -1 : (cmp > 0 ? 1 : 0);
    }

    constexpr int operator<=> (const char* s) const noexcept {
        return (*this) <=> std::string_view {s};
    }

    ///@}

    ///@name string operations
    ///@{

    [[nodiscard]] constexpr std::string_view substr (size_type pos = 0, size_type count = npos) const {
        return view( ).substr(pos, count);
    }

    constexpr size_type copy (char* dest, size_type count, size_type pos = 0) const {
        return view( ).copy(dest, count, pos);
    }

    constexpr int compare (std::string_view sv) const noexcept {
        auto cmp = view( ).compare(sv);
        return cmp < 0 ? -1 : (cmp > 0 ? 1 : 0);
    }

    constexpr bool starts_with (std::string_view sv) const noexcept {
        return view( ).starts_with(sv);
    }

    constexpr bool starts_with (char ch) const noexcept {
        return view( ).starts_with(ch);
    }

    constexpr bool ends_with (std::string_view sv) const noexcept {
        return view( ).ends_with(sv);
    }

    constexpr bool ends_with (char ch) const noexcept {
        return view( ).ends_with(ch);
    }

    constexpr bool contains (std::string_view sv) const noexcept {
        return view( ).find(sv) != npos;
    }

    constexpr bool contains (char ch) const noexcept {
        return view( ).find(ch) != npos;
    }

    constexpr size_type find (std::string_view sv, size_type pos = 0) const noexcept {
        return view( ).find(sv, pos);
    }

    constexpr size_type find (char ch, size_type pos = 0) const noexcept {
        return view( ).find(ch, pos);
    }

    ///@}

#if FMT_SUPPORT
    ///@name format operations
    ///@{

    /**
     * @brief Appends formatted string using fmt library.
     *
     * @tparam T Parameter pack types for format arguments
     * @param fmt Format string
     * @param args Arguments to format
     * @return Reference to this object for chaining
     */
    template<typename... T>
    constexpr len_string_adapter& formatAppend (fmt::format_string<T...> fmt, T&&... args) {
        return append(fmt::format(fmt, std::forward<T>(args)...));
    }

    /**
     * @brief Assigns formatted string using fmt library (clears then appends).
     *
     * @tparam T Parameter pack types for format arguments
     * @param fmt Format string
     * @param args Arguments to format
     * @return Reference to this object for chaining
     */
    template<typename... T>
    constexpr len_string_adapter& formatAssign (fmt::format_string<T...> fmt, T&&... args) {
        return assign(fmt::format(fmt, std::forward<T>(args)...));
    }

    ///@}
#endif

#if STD_FORMAT_SUPPORT
    ///@name format operations (std::format)
    ///@{

    /**
     * @brief Appends formatted string using std::format library.
     *
     * @tparam T Parameter pack types for format arguments
     * @param fmt Format string
     * @param args Arguments to format
     * @return Reference to this object for chaining
     */
    template<typename... T>
    constexpr len_string_adapter& formatAppend (std::format_string<T...> fmt, T&&... args) {
        return append(std::format(fmt, std::forward<T>(args)...));
    }

    /**
     * @brief Assigns formatted string using std::format library (clears then appends).
     *
     * @tparam T Parameter pack types for format arguments
     * @param fmt Format string
     * @param args Arguments to format
     * @return Reference to this object for chaining
     */
    template<typename... T>
    constexpr len_string_adapter& formatAssign (std::format_string<T...> fmt, T&&... args) {
        return assign(std::format(fmt, std::forward<T>(args)...));
    }

    ///@}
#endif

private:
    pointer         buffer_;    ///< Pointer to the character buffer
    const size_type capacity_;  ///< Maximum capacity of the buffer
    len_type&       length_;    ///< Reference to external length variable
};
}  // namespace wbr
#if IOSTREAM_SUPPORT
    #include <iostream>

template<std::unsigned_integral SZ, BoundCheckStrategy bc>
std::ostream& operator<< (std::ostream& str, const wbr::len_string_adapter<SZ, bc>& s) {
    str << s.view( );
    return str;
}
#endif
