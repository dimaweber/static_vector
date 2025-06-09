#pragma once

#include <fmt/format.h>

#include <algorithm>
#include <array>
#include <concepts>
#include <cstring>
#include <iostream>
#include <iterator>
#include <string_view>

#if defined(__GNUC__) && __GNUC__ < 12
    #define EXPECT_SKL_EQ(a, b) EXPECT_TRUE(a == b)
#else
    #define EXPECT_SKL_EQ(a, b) EXPECT_EQ(a, b)
#endif

#define TRUNCATE_ON_OVERFLOW 1
#ifndef TRUNCATE_ON_OVERFLOW
    #define TRUNCATE_ON_OVERFLOW 1
#endif
namespace skl
{
/**
 * @brief Fixed size string interface.

 * Unlinke std::string, which stores its value in heap, skl::string stores its value on stack.
 * Interface of both std:: and skl:: classes are same.
 *
 * Main area of use for skl::string is rapid-created fixed length string, when allocating on heap
 * is too expansive in compare to strings lifetime
 *
 * This class is abstract interface for 2 sublcasses: fixed size string and pre-allocated string
 * @see skl::fs_string
 * @see skl::pa_string
 */
class string
{
// #define MY_STRING_CONSTEXPR constexpr
#define MY_STRING_CONSTEXPR inline
public:
    virtual ~string( )              = default;
    using iterator                  = char*;
    using const_iterator            = const char*;
    using reverse_iterator          = std::reverse_iterator<iterator>;
    using const_reverse_iterator    = std::reverse_iterator<const_iterator>;
    using size_type                 = size_t;
    static constexpr size_type npos = size_type(-1);

    MY_STRING_CONSTEXPR string& operator= (std::string_view sv);
    MY_STRING_CONSTEXPR string& operator= (const std::string& str);
    MY_STRING_CONSTEXPR string& operator= (const char* cstr);
    MY_STRING_CONSTEXPR string& operator= (const string& mystr);

    /** https://en.cppreference.com/w/cpp/string/basic_string/assign */
    template<class InputIt, typename = std::_RequireInputIter<InputIt>>
    MY_STRING_CONSTEXPR string& assign(InputIt first, InputIt last);
    MY_STRING_CONSTEXPR string& assign(size_type count, char c);
    MY_STRING_CONSTEXPR string& assign(std::string_view sv);
    MY_STRING_CONSTEXPR string& assign(std::string_view sv, size_type pos, size_type count = npos);
    MY_STRING_CONSTEXPR string& assign(const char* str);
    MY_STRING_CONSTEXPR string& assign(const char* str, size_type count);
    MY_STRING_CONSTEXPR string& assign(std::initializer_list<char> ilist);

    /** https://en.cppreference.com/w/cpp/string/basic_string/at */
    MY_STRING_CONSTEXPR char  at(size_type pos) const;
    MY_STRING_CONSTEXPR char& at(size_type pos);

    /** https://en.cppreference.com/w/cpp/string/basic_string/operator_at */
    MY_STRING_CONSTEXPR char  operator[] (size_type pos) const;
    MY_STRING_CONSTEXPR char& operator[] (size_type pos);

    /** https://en.cppreference.com/w/cpp/string/basic_string/front */
    MY_STRING_CONSTEXPR const char& front( ) const;
    MY_STRING_CONSTEXPR char&       front( );

    /** https://en.cppreference.com/w/cpp/string/basic_string/back */
    MY_STRING_CONSTEXPR const char& back( ) const;
    MY_STRING_CONSTEXPR char&       back( );

    /** https://en.cppreference.com/w/cpp/string/basic_string/data */
    MY_STRING_CONSTEXPR virtual const char* data( ) const noexcept = 0;
    MY_STRING_CONSTEXPR virtual char*       data( ) noexcept       = 0;

    /** https://en.cppreference.com/w/cpp/string/basic_string/c_str */
    MY_STRING_CONSTEXPR const char* c_str( ) const noexcept;

    /** https://en.cppreference.com/w/cpp/string/basic_string/operator_basic_string_view */
    MY_STRING_CONSTEXPR std::string_view view( ) const noexcept;
    MY_STRING_CONSTEXPR                  operator std::string_view ( ) const noexcept;
    MY_STRING_CONSTEXPR std::string std_str( ) const noexcept;

    MY_STRING_CONSTEXPR iterator               begin( ) noexcept;
    MY_STRING_CONSTEXPR const_iterator         begin( ) const noexcept;
    MY_STRING_CONSTEXPR iterator               end( ) noexcept;
    MY_STRING_CONSTEXPR const_iterator         end( ) const noexcept;
    MY_STRING_CONSTEXPR const_iterator         cbegin( ) const;
    MY_STRING_CONSTEXPR const_iterator         cend( ) const;
    MY_STRING_CONSTEXPR reverse_iterator       rbegin( );
    MY_STRING_CONSTEXPR reverse_iterator       rend( );
    MY_STRING_CONSTEXPR const_reverse_iterator crbegin( ) const;
    MY_STRING_CONSTEXPR const_reverse_iterator crend( ) const;

    /** https://en.cppreference.com/w/cpp/string/basic_string/empty */
    MY_STRING_CONSTEXPR bool empty( ) const noexcept;

    /** https://en.cppreference.com/w/cpp/string/basic_string/size */
    MY_STRING_CONSTEXPR virtual size_type length( ) const noexcept;
    MY_STRING_CONSTEXPR size_type         size( ) const noexcept;

    /** https://en.cppreference.com/w/cpp/string/basic_string/max_size */
    MY_STRING_CONSTEXPR virtual size_type max_size( ) const noexcept = 0;

    /** https://en.cppreference.com/w/cpp/string/basic_string/capacity */
    MY_STRING_CONSTEXPR virtual size_type capacity( ) const noexcept = 0;

    MY_STRING_CONSTEXPR size_type free_space( ) const noexcept;

    /** https://en.cppreference.com/w/cpp/string/basic_string/reserve */
    MY_STRING_CONSTEXPR virtual void reserve(size_type /*new_cap*/) = 0;

    /** https://en.cppreference.com/w/cpp/string/basic_string/shrink_to_fit */
    virtual void shrink_to_fit( ) = 0;

    /** https://en.cppreference.com/w/cpp/string/basic_string/clear */
    MY_STRING_CONSTEXPR void clear( ) noexcept;

    /** https://en.cppreference.com/w/cpp/string/basic_string/insert */
    template<class InputIt, typename = std::_RequireInputIter<InputIt>>
    MY_STRING_CONSTEXPR iterator insert(const_iterator pos, InputIt first, InputIt last);
    MY_STRING_CONSTEXPR iterator insert(const_iterator pos, char ch);
    MY_STRING_CONSTEXPR iterator insert(const_iterator pos, size_type count, char ch);
    MY_STRING_CONSTEXPR string&  insert(size_type index, size_type count, char ch);
    MY_STRING_CONSTEXPR iterator insert(const_iterator pos, std::initializer_list<char> ilist);
    MY_STRING_CONSTEXPR string&  insert(size_type index, std::string_view sv);
    MY_STRING_CONSTEXPR string&  insert(size_type index, std::string_view sv, size_type index_str, size_type count = npos);
    MY_STRING_CONSTEXPR string&  insert(size_type index, const char* cstr);
    MY_STRING_CONSTEXPR string&  insert(size_type index, const char* cstr, size_type count);
    MY_STRING_CONSTEXPR string&  insert(size_type index, const std::string& str);
    MY_STRING_CONSTEXPR string&  insert(size_type index, const std::string& str, size_type index_str, size_type count = npos);
    MY_STRING_CONSTEXPR string&  insert(size_type index, const skl::string& mystr);
    MY_STRING_CONSTEXPR string&  insert(size_type index, const skl::string& mystr, size_type index_str, size_type count = npos);

    /** https://en.cppreference.com/w/cpp/string/basic_string/erase */
    MY_STRING_CONSTEXPR string&  erase(size_type index = 0, size_type count = npos);
    MY_STRING_CONSTEXPR iterator erase(const_iterator position);
    MY_STRING_CONSTEXPR iterator erase(const_iterator first, const_iterator last);

    /** https://en.cppreference.com/w/cpp/string/basic_string/push_back */
    MY_STRING_CONSTEXPR void push_back(char c);

    /** https://en.cppreference.com/w/cpp/string/basic_string/pop_back */
    MY_STRING_CONSTEXPR void pop_back( );

    /** https://en.cppreference.com/w/cpp/string/basic_string/append */
    template<class InputIt, typename = std::_RequireInputIter<InputIt>>
    MY_STRING_CONSTEXPR string& append(InputIt first, InputIt last);
    MY_STRING_CONSTEXPR string& append(size_type count, char ch);
    //    MY_STRING_CONSTEXPR string& append(char ch);
    MY_STRING_CONSTEXPR string& append(const std::string& str);
    MY_STRING_CONSTEXPR string& append(const string& mystr);
    MY_STRING_CONSTEXPR string& append(const char* cstr);
    MY_STRING_CONSTEXPR string& append(std::string_view sv);
    MY_STRING_CONSTEXPR string& append(const std::string& str, size_type pos, size_type count);
    MY_STRING_CONSTEXPR string& append(const string& mystr, size_type pos, size_type count);
    MY_STRING_CONSTEXPR string& append(std::string_view sv, size_type pos, size_type count);
    MY_STRING_CONSTEXPR string& append(std::string_view sv, size_type count);
    MY_STRING_CONSTEXPR string& append(std::initializer_list<char> ilist);

    /** https://en.cppreference.com/w/cpp/string/basic_string/operator%2B%3D */
    MY_STRING_CONSTEXPR string& operator+= (const string& str);
    MY_STRING_CONSTEXPR string& operator+= (const std::string& str);
    MY_STRING_CONSTEXPR string& operator+= (std::string_view str);
    MY_STRING_CONSTEXPR string& operator+= (const char* str);
    MY_STRING_CONSTEXPR string& operator+= (char);
    MY_STRING_CONSTEXPR string& operator+= (std::initializer_list<char> ilist);

    /** https://en.cppreference.com/w/cpp/string/basic_string/compare */
    MY_STRING_CONSTEXPR int compare(const string& secondR) const;
    MY_STRING_CONSTEXPR int compare(size_type pos1, size_type count1, const string& str) const;
    MY_STRING_CONSTEXPR int compare(size_type pos1, size_type count1, const string& str, size_type pos2, size_type count2 = npos) const;
    MY_STRING_CONSTEXPR int compare(const std::string& str) const noexcept;
    MY_STRING_CONSTEXPR int compare(size_type pos1, size_type count1, const std::string& str) const;
    MY_STRING_CONSTEXPR int compare(size_type pos1, size_type count1, const std::string& str, size_type pos2, size_type count2 = npos) const;
    MY_STRING_CONSTEXPR int compare(const char* s) const;
    MY_STRING_CONSTEXPR int compare(size_type pos1, size_type count1, const char* s) const;
    MY_STRING_CONSTEXPR int compare(size_type pos1, size_type count1, const char* s, size_type count2) const;
    MY_STRING_CONSTEXPR int compare(std::string_view t) const noexcept;
    MY_STRING_CONSTEXPR int compare(size_type pos1, size_type count1, std::string_view t) const;
    MY_STRING_CONSTEXPR int compare(size_type pos1, size_type count1, std::string_view t, size_type pos2, size_type count2 = npos) const;

    /** https://en.cppreference.com/w/cpp/string/basic_string/starts_with */
    MY_STRING_CONSTEXPR bool starts_with(std::string_view secondP) const noexcept;
    MY_STRING_CONSTEXPR bool starts_with(char ch) const noexcept;
    MY_STRING_CONSTEXPR bool starts_with(const char*) const;

    /** https://en.cppreference.com/w/cpp/string/basic_string/ends_with */
    MY_STRING_CONSTEXPR bool ends_with(std::string_view sv) const noexcept;
    MY_STRING_CONSTEXPR bool ends_with(char ch) const noexcept;
    MY_STRING_CONSTEXPR bool ends_with(const char* s) const;

    /** https://en.cppreference.com/w/cpp/string/basic_string/contains */
    MY_STRING_CONSTEXPR bool contains(std::string_view sv) const noexcept;
    MY_STRING_CONSTEXPR bool contains(char ch) const noexcept;
    MY_STRING_CONSTEXPR bool contains(const char* s) const;

    /** https://en.cppreference.com/w/cpp/string/basic_string/replace */
    template<class InputIt, typename = std::_RequireInputIter<InputIt>>
    MY_STRING_CONSTEXPR string& replace(const_iterator first, const_iterator last, InputIt first2, InputIt last2);
    MY_STRING_CONSTEXPR string& replace(size_type pos, size_type count, const std::string& str);
    MY_STRING_CONSTEXPR string& replace(const_iterator first, const_iterator last, const std::string& str);
    MY_STRING_CONSTEXPR string& replace(size_type pos, size_type count, const std::string& str, size_type pos2, size_type count2 = npos);
    MY_STRING_CONSTEXPR string& replace(size_type pos, size_type count, const char* cstr, size_type count2);
    MY_STRING_CONSTEXPR string& replace(const_iterator first, const_iterator last, const char* cstr, size_type count2);
    MY_STRING_CONSTEXPR string& replace(size_type pos, size_type count, const char* cstr);
    MY_STRING_CONSTEXPR string& replace(const_iterator first, const_iterator last, const char* cstr);
    MY_STRING_CONSTEXPR string& replace(size_type pos, size_type count, size_type count2, char ch);
    MY_STRING_CONSTEXPR string& replace(const_iterator first, const_iterator last, size_type count2, char ch);
    MY_STRING_CONSTEXPR string& replace(const_iterator first, const_iterator last, std::initializer_list<char> ilist);
    MY_STRING_CONSTEXPR string& replace(size_type pos, size_type count, std::string_view sv);
    MY_STRING_CONSTEXPR string& replace(const_iterator first, const_iterator last, std::string_view t);
    MY_STRING_CONSTEXPR string& replace(size_type pos, size_type count, std::string_view t, size_type pos2, size_type count2 = npos);

    /** https://en.cppreference.com/w/cpp/string/basic_string/substr */
    MY_STRING_CONSTEXPR std::string_view substr(size_type pos = 0, size_type count = npos) const;

    /** https://en.cppreference.com/w/cpp/string/basic_string/copy */
    MY_STRING_CONSTEXPR size_type copy(char* dest, size_type count, size_type pos = 0) const;

    /** https://en.cppreference.com/w/cpp/string/basic_string/resize */
    MY_STRING_CONSTEXPR void resize(size_type count);
    MY_STRING_CONSTEXPR void resize(size_type count, char ch);

    /** https://en.cppreference.com/w/cpp/string/basic_string/swap */
    MY_STRING_CONSTEXPR void swap(string& other) noexcept;

    /** https://en.cppreference.com/w/cpp/string/basic_string/find */
    MY_STRING_CONSTEXPR size_type find(const std::string& str, size_type pos = 0) const noexcept;
    MY_STRING_CONSTEXPR size_type find(const char* s, size_type pos, size_type count) const;
    MY_STRING_CONSTEXPR size_type find(const char* pattern, size_type startPos = 0) const;
    MY_STRING_CONSTEXPR size_type find(char c, size_type startPos = 0) const noexcept;
    MY_STRING_CONSTEXPR size_type find(std::string_view t, size_type pos = 0) const noexcept;

    MY_STRING_CONSTEXPR size_type find_first_of(const string& str, size_type pos = 0) const noexcept;
    MY_STRING_CONSTEXPR size_type find_first_of(const std::string& str, size_type pos = 0) const noexcept;
    MY_STRING_CONSTEXPR size_type find_first_of(const char* s, size_type pos, size_type count) const;
    MY_STRING_CONSTEXPR size_type find_first_of(const char* s, size_type pos = 0) const;
    MY_STRING_CONSTEXPR size_type find_first_of(char ch, size_type pos = 0) const noexcept;
    MY_STRING_CONSTEXPR size_type find_first_of(std::string_view sv, size_type startPos = 0) const noexcept;

    MY_STRING_CONSTEXPR size_type find_first_not_of(const string& str, size_type pos = 0) const noexcept;
    MY_STRING_CONSTEXPR size_type find_first_not_of(const std::string& str, size_type pos = 0) const noexcept;
    MY_STRING_CONSTEXPR size_type find_first_not_of(const char* s, size_type pos, size_type count) const;
    MY_STRING_CONSTEXPR size_type find_first_not_of(const char* s, size_type pos = 0) const;
    MY_STRING_CONSTEXPR size_type find_first_not_of(char ch, size_type pos = 0) const noexcept;
    MY_STRING_CONSTEXPR size_type find_first_not_of(std::string_view sv, size_type startPos = 0) const noexcept;

    MY_STRING_CONSTEXPR size_type find_last_of(const string& str, size_type pos = 0) const noexcept;
    MY_STRING_CONSTEXPR size_type find_last_of(const std::string& str, size_type pos = 0) const noexcept;
    MY_STRING_CONSTEXPR size_type find_last_of(const char* s, size_type pos, size_type count) const;
    MY_STRING_CONSTEXPR size_type find_last_of(const char* s, size_type pos = 0) const;
    MY_STRING_CONSTEXPR size_type find_last_of(char ch, size_type pos = npos) const noexcept;
    MY_STRING_CONSTEXPR size_type find_last_of(std::string_view sv, size_type startPos = 0) const noexcept;

    MY_STRING_CONSTEXPR size_type find_last_not_of(const string& str, size_type pos = 0) const noexcept;
    MY_STRING_CONSTEXPR size_type find_last_not_of(const std::string& str, size_type pos = 0) const noexcept;
    MY_STRING_CONSTEXPR size_type find_last_not_of(const char* s, size_type pos, size_type count) const;
    MY_STRING_CONSTEXPR size_type find_last_not_of(const char* s, size_type pos = 0) const;
    MY_STRING_CONSTEXPR size_type find_last_not_of(char ch, size_type pos = npos) const noexcept;
    MY_STRING_CONSTEXPR size_type find_last_not_of(std::string_view sv, size_type startPos = 0) const noexcept;

    MY_STRING_CONSTEXPR bool operator== (const skl::string& mystr) const;
    MY_STRING_CONSTEXPR bool operator== (std::string_view sv) const;
    MY_STRING_CONSTEXPR bool operator== (const std::string& str) const;
    MY_STRING_CONSTEXPR bool operator== (const char* cstr) const;

    MY_STRING_CONSTEXPR int operator<=> (const skl::string& mystr) const;
    MY_STRING_CONSTEXPR int operator<=> (const std::string& str) const;
    MY_STRING_CONSTEXPR int operator<=> (std::string_view sv) const;
    MY_STRING_CONSTEXPR int operator<=> (const char* cstr) const;

    template<typename... T>
    MY_STRING_CONSTEXPR skl::string& formatAppend (fmt::format_string<T...> fmt, T&&... args)
    {
#if FMT_VERSION < 90000
        return append(fmt::format(fmt, std::forward<T>(args)...));
#else
        auto out = fmt::format_to_n(end( ), free_space( ), fmt, std::forward<T>(args)...);
        tail     = out.out;
        *tail    = '\0';
        return *this;
#endif
    }

    template<typename... T>
    MY_STRING_CONSTEXPR skl::string& formatAssign (fmt::format_string<T...> fmt, T&&... args)
    {
#if FMT_VERSION < 90000
        return assign(fmt::format(fmt, std::forward<T>(args)...));
#else
        auto out = fmt::format_to_n(begin( ), max_size( ), fmt, std::forward<T>(args)...);
        tail     = out.out;
        *tail    = '\0';
        return *this;
#endif
    }

    bool AsInt(int* intP);
    int  AsInt( );

protected:
    iterator tail;

private:
    MY_STRING_CONSTEXPR size_type insertHelper(size_type index, size_type count);
};

/**
 * @brief skl::string adapter for C-strings (char [] / char*)
 * Allow to work with char c-array's as with usual std/skl string - with length safeness and overflow check on the fly.
 */
class pa_string : public string
{
public:
    pa_string(char* cstr, string::size_type size) : bufferSize(size), buffer(cstr)
    {
        buffer[bufferSize - 1] = 0;
        tail                   = begin( ) + bufferSize;
        tail                   = begin( ) + find('\0');
    }

    template<size_t S>
    explicit pa_string(std::array<char, S>& a) : bufferSize(a.size( )), buffer(a.data( ))
    {
        buffer[bufferSize - 1] = 0;
        tail                   = begin( ) + bufferSize;
        tail                   = begin( ) + find('\0');
    }

    template<size_t S>
    explicit pa_string(char (&arr)[S]) : bufferSize(S), buffer(arr)
    {
        buffer[bufferSize - 1] = 0;
        tail                   = begin( ) + bufferSize;
        tail                   = begin( ) + find('\0');
    }

    MY_STRING_CONSTEXPR size_type capacity ( ) const noexcept override { return bufferSize - 1; }

    MY_STRING_CONSTEXPR size_type max_size ( ) const noexcept override { return bufferSize - 1; }

    MY_STRING_CONSTEXPR void reserve (size_type /*new_cap*/) override { /* static string can't change it's size and as such don't need reserv implementation */ }

    void shrink_to_fit ( ) override { /* static string can't change it's size and as such don't need shrink_to_fit implementation */ }

    MY_STRING_CONSTEXPR char* data ( ) noexcept override { return buffer; }

    MY_STRING_CONSTEXPR const char* data ( ) const noexcept override { return buffer; }

protected:
    string::size_type bufferSize;
    char*             buffer;
};

/**
 * @brief Fixed size string
 *
 * Allocated MAX_LEN array on stack and works with it as with string
 * @tparam MAX_LEN - maximum string capacity
 */
template<string::size_type MAX_LEN>
class fs_string : public string
{
public:
    fs_string( )
    {
        _buffer.fill(0);
        tail  = _buffer.begin( );
        *tail = '\0';
    }

    explicit fs_string(const fs_string& str) : fs_string( ) { assign(str); }

    explicit fs_string(std::string_view text) : fs_string( ) { assign(text); }

    explicit fs_string(std::string& text) : fs_string( ) { assign(text); }

    fs_string(fs_string<MAX_LEN>&& str) noexcept : fs_string( )
    {
        tail  = std::copy(str.cbegin( ), str.cend( ), begin( ));
        *tail = '\0';
    }

    template<typename... T>
    fs_string(fmt::format_string<T...> f, T&&... args) : fs_string( )
    {
#if FMT_VERSION < 90000
        assign(fmt::format(f, std::forward<T>(args)...));
#else
        auto res = fmt::format_to_n(_buffer.data( ), _buffer.size( ) - 1, f, std::forward<T>(args)...);
        tail     = res.out;
        *tail    = '\0';
#endif
    }

    fs_string<MAX_LEN>& operator= (const fs_string<MAX_LEN>& other)
    {
        assign(other);
        return *this;
    }

    fs_string<MAX_LEN>& operator= (fs_string<MAX_LEN>&& str) noexcept
    {
        if ( &str != this ) {
            tail  = std::copy(str.cbegin( ), str.cend( ), begin( ));
            *tail = '\0';
        }
        return *this;
    }

    fs_string<MAX_LEN>& operator= (std::string& str)
    {
        assign(str);
        return *this;
    }

    fs_string<MAX_LEN>& operator= (std::string_view str)
    {
        assign(str);
        return *this;
    }

    fs_string<MAX_LEN>& operator= (const char* str)
    {
        assign(str);
        return *this;
    }

    fs_string<MAX_LEN>& operator= (const string& str)
    {
        assign(str);
        return *this;
    }

    MY_STRING_CONSTEXPR size_type capacity ( ) const noexcept override { return _buffer.size( ) - 1; }

    MY_STRING_CONSTEXPR size_type max_size ( ) const noexcept override { return _buffer.size( ) - 1; }

    MY_STRING_CONSTEXPR void reserve (size_type /*new_cap*/) override { /* static string can't change it's size and as such don't need reserv implementation */ }

    void shrink_to_fit ( ) override { /* static string can't change it's size and as such don't need shrink_to_fit implementation */ }

    MY_STRING_CONSTEXPR char* data ( ) noexcept override { return _buffer.data( ); }

    MY_STRING_CONSTEXPR const char* data ( ) const noexcept override { return _buffer.data( ); }

private:
    std::array<char, MAX_LEN + 1> _buffer;
};

template<std::derived_from<skl::string> T1, std::derived_from<skl::string> T2>
void swap (T1& a, T2& b) noexcept
{
    a.swap(b);
}

template<std::derived_from<skl::string> T>
auto begin (const T& s) noexcept
{
    return s.begin( );
}

template<std::derived_from<skl::string> T>
auto end (const T& s) noexcept
{
    return s.end( );
}
}  // namespace skl

inline std::ostream& operator<< (std::ostream& stream, const skl::string& s)
{
    std::for_each(s.cbegin( ), s.cend( ), [&stream] (char ch) {
        if ( ch >= 0x20 || ch == '\t' || ch == '\n' )
            stream << ch;
        else
            stream << '#';
    });
    return stream;
}

bool operator== (std::string_view str1, const skl::string& str2);
bool operator== (const std::string& str1, const skl::string& str2);

MY_STRING_CONSTEXPR char& skl::string::at(skl::string::size_type pos)
{
    if ( pos >= length( ) )
        throw std::out_of_range("index out of range");
    return data( )[pos];
}

MY_STRING_CONSTEXPR const char& skl::string::front( ) const
{
    return *cbegin( );
}

MY_STRING_CONSTEXPR char& skl::string::front( )
{
    return *begin( );
}

MY_STRING_CONSTEXPR const char& skl::string::back( ) const
{
    return *std::prev(cend( ));
}

MY_STRING_CONSTEXPR char& skl::string::back( )
{
    return *std::prev(end( ));
}

MY_STRING_CONSTEXPR const char* skl::string::c_str( ) const noexcept
{
    *tail = '\0';
    return data( );
}

MY_STRING_CONSTEXPR std::string_view skl::string::view( ) const noexcept
{
    return {data( ), length( )};
}

MY_STRING_CONSTEXPR skl::string::operator std::string_view ( ) const noexcept
{
    return view( );
}

MY_STRING_CONSTEXPR skl::string::iterator skl::string::begin( ) noexcept
{
    return data( );
}

MY_STRING_CONSTEXPR skl::string::const_iterator skl::string::begin( ) const noexcept
{
    return cbegin( );
}

MY_STRING_CONSTEXPR skl::string::iterator skl::string::end( ) noexcept
{
    return tail;
}

MY_STRING_CONSTEXPR skl::string::const_iterator skl::string::end( ) const noexcept
{
    return cend( );
}

MY_STRING_CONSTEXPR skl::string::const_iterator skl::string::cbegin( ) const
{
    return data( );
}

MY_STRING_CONSTEXPR skl::string::const_iterator skl::string::cend( ) const
{
    return tail;
}

MY_STRING_CONSTEXPR skl::string::reverse_iterator skl::string::rbegin( )
{
    return reverse_iterator {end( )};
}

MY_STRING_CONSTEXPR skl::string::reverse_iterator skl::string::rend( )
{
    return reverse_iterator {data( )};
}

MY_STRING_CONSTEXPR skl::string::const_reverse_iterator skl::string::crbegin( ) const
{
    return const_reverse_iterator {cend( )};
}

MY_STRING_CONSTEXPR skl::string::const_reverse_iterator skl::string::crend( ) const
{
    return const_reverse_iterator {data( )};
}

MY_STRING_CONSTEXPR bool skl::string::empty( ) const noexcept
{
    return cbegin( ) == cend( );
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::length( ) const noexcept
{
    return std::distance(cbegin( ), cend( ));
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::size( ) const noexcept
{
    return length( );
}

MY_STRING_CONSTEXPR char skl::string::operator[] (skl::string::size_type pos) const
{
    return view( )[pos];
}

MY_STRING_CONSTEXPR char& skl::string::operator[] (skl::string::size_type pos)
{
    return data( )[pos];
}

template<class InputIt, typename>
MY_STRING_CONSTEXPR skl::string& skl::string::assign(InputIt first, InputIt last)
{
    InputIt iter {last};
    if ( std::abs(std::distance(first, last)) > (ssize_t)capacity( ) ) {
#if TRUNCATE_ON_OVERFLOW
        iter = std::next(first, capacity( ));
#else
        throw std::length_error("too long");
#endif
    }

    tail  = std::copy(first, iter, begin( ));
    *tail = '\0';
    return *this;
}

MY_STRING_CONSTEXPR skl::string& skl::string::assign(skl::string::size_type count, char c)
{
    size_type len = count;
#if TRUNCATE_ON_OVERFLOW
    len = std::min(capacity( ), count);
#else
    if ( count > capacity( ) )
        throw std::length_error("too long");
#endif
    tail  = std::fill_n(begin( ), len, c);
    *tail = '\0';
    return *this;
}

MY_STRING_CONSTEXPR skl::string& skl::string::assign(std::string_view sv)
{
    return assign(sv.begin( ), sv.end( ));
}

MY_STRING_CONSTEXPR skl::string& skl::string::assign(std::string_view sv, skl::string::size_type pos, skl::string::size_type count)
{
    if ( pos > sv.length( ) )
        throw std::out_of_range("out of range");
    if ( count == npos )
        count = sv.length( );
    count = std::min(pos + count, sv.length( )) - pos;
    return assign(std::next(sv.cbegin( ), pos), std::next(sv.cbegin( ), pos + count));
}

MY_STRING_CONSTEXPR skl::string& skl::string::assign(const char* str)
{
    return assign(std::string_view {str});
}

MY_STRING_CONSTEXPR skl::string& skl::string::assign(const char* cstr, size_type count)
{
    return assign(cstr, cstr + count);  // converts to iterators
}

MY_STRING_CONSTEXPR skl::string& skl::string::assign(std::initializer_list<char> list)
{
    return assign(list.begin( ), list.end( ));
}

MY_STRING_CONSTEXPR char skl::string::at(skl::string::size_type pos) const
{
    if ( pos > capacity( ) )
        throw std::out_of_range("index out of range");
    return data( )[pos];
}

MY_STRING_CONSTEXPR void skl::string::clear( ) noexcept
{
    tail  = begin( );
    *tail = '\0';
}

MY_STRING_CONSTEXPR void skl::string::push_back(char c)
{
    if ( const size_type idx = length( ); idx == capacity( ) )
        throw std::length_error("index out of range");
    *tail = c;
    std::advance(tail, 1);
    *tail = '\0';
}

MY_STRING_CONSTEXPR void skl::string::pop_back( )
{
    erase(std::prev(end( )));
}

// MY_STRING_CONSTEXPR skl::string& skl::string::append(char ch)
//{
//     return append(1, ch);
// }

MY_STRING_CONSTEXPR skl::string& skl::string::append(skl::string::size_type count, char ch)
{
    size_type len = std::min(free_space( ), count);
    tail          = std::fill_n(end( ), len, ch);
    *tail         = '\0';
    return *this;
}

MY_STRING_CONSTEXPR skl::string& skl::string::append(const skl::string& mystr)
{
    return append(mystr.view( ));
}

MY_STRING_CONSTEXPR skl::string& skl::string::append(const std::string& str)
{
    return append(std::string_view {str});
}

MY_STRING_CONSTEXPR skl::string& skl::string::append(std::string_view sv)
{
    return append(sv.cbegin( ), sv.cend( ));
}

MY_STRING_CONSTEXPR skl::string& skl::string::append(const char* cstr)
{
    return append(std::string_view {cstr});
}

MY_STRING_CONSTEXPR skl::string& skl::string::append(const std::string& str, size_type pos, size_type count)
{
    return append(std::string_view {str}, pos, count);
}

MY_STRING_CONSTEXPR skl::string& skl::string::append(const skl::string& mystr, skl::string::size_type pos, skl::string::size_type count)
{
    return append(mystr.view( ), pos, count);
}

MY_STRING_CONSTEXPR skl::string& skl::string::append(std::string_view sv, skl::string::size_type pos, skl::string::size_type count)
{
    return append(std::next(sv.cbegin( ), pos), std::next(sv.cbegin( ), pos + count));
}

MY_STRING_CONSTEXPR skl::string& skl::string::append(std::string_view sv, skl::string::size_type count)
{
    return append(sv, 0, count);
}

template<class InputIt, typename>
MY_STRING_CONSTEXPR skl::string& skl::string::append(InputIt first, InputIt last)
{
    tail  = std::copy(first, last, end( ));
    *tail = '\0';
    return *this;
}

MY_STRING_CONSTEXPR skl::string& skl::string::append(std::initializer_list<char> ilist)
{
    return append(ilist.begin( ), ilist.end( ));
}

MY_STRING_CONSTEXPR skl::string& skl::string::operator= (std::string_view sv)
{
    return assign(sv);
}

MY_STRING_CONSTEXPR skl::string& skl::string::operator= (const std::string& str)
{
    return assign(str);
}

MY_STRING_CONSTEXPR skl::string& skl::string::operator= (const char* cstr)
{
    return assign(cstr);
}

MY_STRING_CONSTEXPR skl::string& skl::string::operator= (const skl::string& mystr)
{
    return assign(mystr);
}

MY_STRING_CONSTEXPR skl::string& skl::string::erase(size_t index, skl::string::size_type count)
{
    if ( index > length( ) )
        throw std::out_of_range("index out of range");
    if ( count == npos )
        count = length( ) - index;
    else
        count = std::min(length( ) - index, count);
    tail  = std::copy(std::next(begin( ), index + count), end( ), begin( ) + index);
    *tail = '\0';
    return *this;
}

MY_STRING_CONSTEXPR skl::string::iterator skl::string::erase(skl::string::const_iterator position)
{
    if ( position < cbegin( ) || position >= cend( ) )
        return end( );
    iterator pos = const_cast<iterator>(position);
    tail         = std::copy(std::next(pos), end( ), pos);
    *tail        = '\0';
    return std::next(pos);
}

MY_STRING_CONSTEXPR skl::string::iterator skl::string::erase(skl::string::const_iterator first, skl::string::const_iterator last)
{
    if ( first < begin( ) || first >= end( ) )
        return end( );
    if ( first > last )
        return end( );
    if ( first == last )
        return const_cast<iterator>(std::next(first));
    if ( last > end( ) )
        last = end( );
    tail  = std::copy(const_cast<iterator>(last), end( ), const_cast<iterator>(first));
    *tail = '\0';
    return const_cast<iterator>(std::next(first));
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::free_space( ) const noexcept
{
    return capacity( ) - length( );
}

MY_STRING_CONSTEXPR skl::string& skl::string::operator+= (const skl::string& str)
{
    return append(str);
}

MY_STRING_CONSTEXPR skl::string& skl::string::operator+= (const std::string& str)
{
    return append(str);
}

MY_STRING_CONSTEXPR skl::string& skl::string::operator+= (std::string_view str)
{
    return append(str);
}

MY_STRING_CONSTEXPR skl::string& skl::string::operator+= (const char* str)
{
    return append(str);
}

MY_STRING_CONSTEXPR skl::string& skl::string::operator+= (char ch)
{
    push_back(ch);
    return *this;
}

MY_STRING_CONSTEXPR skl::string& skl::string::operator+= (std::initializer_list<char> ilist)
{
    return append(ilist);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::insertHelper(size_t index, size_t count)
{
    if ( index + count > capacity( ) )
        count = capacity( ) - index;

    size_t tailLen = length( ) - index;
    if ( length( ) + count > capacity( ) )
        tailLen = capacity( ) - (index + count);

    tail += std::min(free_space( ), count);
    std::copy_backward(begin( ) + index, begin( ) + index + tailLen, tail);
    *tail = '\0';

    return count;
}

MY_STRING_CONSTEXPR skl::string& skl::string::insert(size_type index, size_type count, char ch)
{
    insert(std::next(begin( ), index), count, ch);
    return *this;
}

MY_STRING_CONSTEXPR skl::string& skl::string::insert(size_t index, const char* cstr)
{
    return insert(index, std::string_view {cstr});
}

MY_STRING_CONSTEXPR skl::string& skl::string::insert(size_type index, const std::string& str)
{
    return insert(index, std::string_view {str});
}

MY_STRING_CONSTEXPR skl::string& skl::string::insert(size_type index, std::string_view sv)
{
    insert(std::next(begin( ), index), sv.cbegin( ), sv.cend( ));
    return *this;
}

MY_STRING_CONSTEXPR skl::string& skl::string::insert(size_type index, const std::string& str, size_type index_str, size_type count)
{
    return insert(index, std::string_view {str}, index_str, count);
}

MY_STRING_CONSTEXPR skl::string& skl::string::insert(size_type index, const skl::string& mystr)
{
    insert(index, mystr.view( ));
    return *this;
}

MY_STRING_CONSTEXPR skl::string& skl::string::insert(size_type index, const skl::string& mystr, size_type index_str, size_type count)
{
    insert(index, mystr.view( ), index_str, count);
    return *this;
}

MY_STRING_CONSTEXPR skl::string::iterator skl::string::insert(skl::string::const_iterator pos, char ch)
{
    return insert(pos, 1, ch);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find_first_of(char ch, skl::string::size_type pos) const noexcept
{
    return view( ).find_first_of(ch, pos);
}

MY_STRING_CONSTEXPR skl::string::iterator skl::string::insert(skl::string::const_iterator pos, skl::string::size_type count, char ch)
{
    size_type index = pos - cbegin( );
    count           = insertHelper(index, count);
    std::fill_n(begin( ) + index, count, ch);
    return begin( ) + index;
}

MY_STRING_CONSTEXPR skl::string& skl::string::insert(skl::string::size_type index, const char* cstr, skl::string::size_type count)
{
    return insert(index, std::string_view {cstr, count});
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find_last_of(char ch, skl::string::size_type pos) const noexcept
{
    return view( ).find_last_of(ch, pos);
}

template<class InputIt, typename>
MY_STRING_CONSTEXPR skl::string::iterator skl::string::insert(skl::string::const_iterator pos, InputIt first, InputIt last)
{
    const size_type index = std::distance(cbegin( ), pos);
    size_type       count = last - first;
    count                 = insertHelper(index, count);
    std::copy_n(first, count, begin( ) + index);
    return begin( ) + index;
}

MY_STRING_CONSTEXPR skl::string::iterator skl::string::insert(const_iterator pos, std::initializer_list<char> ilist)
{
    return insert(pos, ilist.begin( ), ilist.end( ));
}

MY_STRING_CONSTEXPR bool skl::string::starts_with(std::string_view secondP) const noexcept
{
    return view( ).starts_with(secondP);
}

MY_STRING_CONSTEXPR bool skl::string::starts_with(char ch) const noexcept
{
    return view( ).starts_with(ch);
}

MY_STRING_CONSTEXPR bool skl::string::starts_with(const char* s) const
{
    if ( s == nullptr )
        return false;
    return view( ).starts_with(std::string_view {s});
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find(const char* pattern, skl::string::size_type startPos) const
{
    return view( ).find(pattern, startPos);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find(char c, skl::string::size_type pos) const noexcept
{
    return view( ).find(c, pos);
}

MY_STRING_CONSTEXPR size_t skl::string::find_first_of(std::string_view s, size_t startPos) const noexcept
{
    return view( ).find_first_of(s, startPos);
}

MY_STRING_CONSTEXPR skl::string& skl::string::replace(skl::string::size_type pos, skl::string::size_type count, std::string_view sv)
{
    return replace(begin( ) + pos, begin( ) + pos + count, sv.cbegin( ), sv.cend( ));
}

MY_STRING_CONSTEXPR skl::string& skl::string::replace(skl::string::const_iterator first, skl::string::const_iterator last, std::string_view sv)
{
    return replace(first, last, sv.begin( ), sv.end( ));
}

MY_STRING_CONSTEXPR skl::string& skl::string::replace(skl::string::size_type pos, skl::string::size_type count, const std::string_view sv, skl::string::size_type pos2, skl::string::size_type count2)
{
    return replace(begin( ) + pos, begin( ) + pos + count, sv.cbegin( ) + pos2, sv.cbegin( ) + pos2 + count2);
}

MY_STRING_CONSTEXPR int skl::string::compare(const std::string& str) const noexcept
{
    return view( ).compare(str);
}

MY_STRING_CONSTEXPR int skl::string::compare(skl::string::size_type pos1, skl::string::size_type count1, const std::string& str) const
{
    return view( ).compare(pos1, count1, str);
}

MY_STRING_CONSTEXPR int skl::string::compare(skl::string::size_type pos1, skl::string::size_type count1, const std::string& str, skl::string::size_type pos2, skl::string::size_type count2) const
{
    return view( ).compare(pos1, count1, str, pos2, count2);
}

MY_STRING_CONSTEXPR int skl::string::compare(const char* s) const
{
    return view( ).compare(s);
}

MY_STRING_CONSTEXPR int skl::string::compare(skl::string::size_type pos1, skl::string::size_type count1, const char* s) const
{
    return view( ).compare(pos1, count1, s);
}

MY_STRING_CONSTEXPR int skl::string::compare(skl::string::size_type pos1, skl::string::size_type count1, const char* s, skl::string::size_type count2) const
{
    return view( ).compare(pos1, count1, s, count2);
}

MY_STRING_CONSTEXPR int skl::string::compare(std::string_view t) const noexcept
{
    return view( ).compare(t);
}

MY_STRING_CONSTEXPR int skl::string::compare(skl::string::size_type pos1, skl::string::size_type count1, std::string_view t) const
{
    return view( ).compare(pos1, count1, t);
}

MY_STRING_CONSTEXPR int skl::string::compare(skl::string::size_type pos1, skl::string::size_type count1, std::string_view t, skl::string::size_type pos2, skl::string::size_type count2) const
{
    return view( ).compare(pos1, count1, t, pos2, count2);
}

MY_STRING_CONSTEXPR int skl::string::compare(skl::string::size_type pos1, skl::string::size_type count1, const skl::string& str) const
{
    return view( ).compare(pos1, count1, str.view( ));
}

MY_STRING_CONSTEXPR int skl::string::compare(skl::string::size_type pos1, skl::string::size_type count1, const skl::string& str, skl::string::size_type pos2, skl::string::size_type count2) const
{
    return view( ).compare(pos1, count1, str.view( ), pos2, count2);
}

MY_STRING_CONSTEXPR bool skl::string::ends_with(std::string_view sv) const noexcept
{
    return view( ).ends_with(sv);
}

MY_STRING_CONSTEXPR bool skl::string::ends_with(char ch) const noexcept
{
    return view( ).ends_with(ch);
}

MY_STRING_CONSTEXPR bool skl::string::ends_with(const char* s) const
{
    if ( s == nullptr )
        return false;
    return view( ).ends_with(std::string_view {s});
}

MY_STRING_CONSTEXPR bool skl::string::contains(std::string_view sv) const noexcept
{
    return find(sv) != skl::string::npos;
}

MY_STRING_CONSTEXPR bool skl::string::contains(char ch) const noexcept
{
    return find(ch) != skl::string::npos;
}

MY_STRING_CONSTEXPR bool skl::string::contains(const char* s) const
{
    return find(s) != skl::string::npos;
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::copy(char* dest, skl::string::size_type count, skl::string::size_type pos) const
{
    return view( ).copy(dest, count, pos);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find(const std::string& str, skl::string::size_type pos) const noexcept
{
    return view( ).find(str, pos);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find(const char* s, skl::string::size_type pos, skl::string::size_type count) const
{
    return view( ).find(s, pos, count);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find(std::string_view t, skl::string::size_type pos) const noexcept
{
    return view( ).find(t, pos);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find_first_of(const std::string& str, skl::string::size_type pos) const noexcept
{
    return view( ).find_first_of(str, pos);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find_first_of(const skl::string& str, skl::string::size_type pos) const noexcept
{
    return view( ).find_first_of(str.view( ), pos);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find_first_of(const char* s, skl::string::size_type pos, skl::string::size_type count) const
{
    return view( ).find_first_of(s, pos, count);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find_first_of(const char* s, skl::string::size_type pos) const
{
    return view( ).find_first_of(s, pos);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find_last_of(const skl::string& str, skl::string::size_type pos) const noexcept
{
    return view( ).find_last_of(str.view( ), pos);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find_last_of(const std::string& str, skl::string::size_type pos) const noexcept
{
    return view( ).find_last_of(str, pos);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find_last_of(const char* s, skl::string::size_type pos, skl::string::size_type count) const
{
    return view( ).find_last_of(s, pos, count);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find_last_of(const char* s, skl::string::size_type pos) const
{
    return view( ).find_last_of(s, pos);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find_last_of(std::string_view sv, skl::string::size_type startPos) const noexcept
{
    return view( ).find_last_of(sv, startPos);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find_first_not_of(const skl::string& str, skl::string::size_type pos) const noexcept
{
    return view( ).find_first_not_of(str.view( ), pos);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find_first_not_of(const std::string& str, skl::string::size_type pos) const noexcept
{
    return view( ).find_first_not_of(str, pos);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find_first_not_of(const char* s, skl::string::size_type pos, skl::string::size_type count) const
{
    return view( ).find_first_not_of(s, pos, count);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find_first_not_of(const char* s, skl::string::size_type pos) const
{
    return view( ).find_first_not_of(s, pos);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find_first_not_of(char ch, skl::string::size_type pos) const noexcept
{
    return view( ).find_first_not_of(ch, pos);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find_first_not_of(std::string_view sv, skl::string::size_type startPos) const noexcept
{
    return view( ).find_first_not_of(sv, startPos);
}

MY_STRING_CONSTEXPR int skl::string::operator<=> (const skl::string& other) const
{
    return compare(other);
}

MY_STRING_CONSTEXPR int skl::string::operator<=> (const std::string& other) const
{
    return compare(other);
}

MY_STRING_CONSTEXPR int skl::string::operator<=> (std::string_view other) const
{
    return compare(other);
}

MY_STRING_CONSTEXPR int skl::string::operator<=> (const char* other) const
{
    return compare(other);
}

template<class InputIt, typename>
MY_STRING_CONSTEXPR skl::string& skl::string::replace(skl::string::const_iterator first, skl::string::const_iterator last, InputIt first2, InputIt last2)
{
    auto count  = std::distance(first, last);
    auto count2 = std::distance(first2, last2);
    auto pos    = std::distance(cbegin( ), first);

    auto iter = std::copy(first2, first2 + std::min(count, count2), begin( ) + pos);
    if ( count < count2 ) {
        insert(iter, first2 + count, last2);
    } else if ( count > count2 ) {
        erase(iter, last);
    }
    return *this;
}

MY_STRING_CONSTEXPR int skl::string::compare(const skl::string& secondR) const
{
    return view( ).compare(secondR.view( ));
}

MY_STRING_CONSTEXPR bool skl::string::operator== (const skl::string& second) const
{
    if ( size( ) != second.size( ) )
        return false;
    return std::equal(cbegin( ), cend( ), second.cbegin( ));
}

MY_STRING_CONSTEXPR bool skl::string::operator== (std::string_view second) const
{
    if ( size( ) != second.size( ) )
        return false;
    return std::equal(cbegin( ), cend( ), second.cbegin( ));
}

MY_STRING_CONSTEXPR bool skl::string::operator== (const std::string& second) const
{
    if ( size( ) != second.size( ) )
        return false;
    return std::equal(cbegin( ), cend( ), second.begin( ));
}

MY_STRING_CONSTEXPR bool skl::string::operator== (const char* secondP) const
{
    //    if ( size( ) != strlen(secondP) )
    //        return false;
    return view( ) == secondP;
}

MY_STRING_CONSTEXPR skl::string& skl::string::insert(skl::string::size_type index, std::string_view sv, skl::string::size_type index_str, skl::string::size_type count)
{
    if ( count == npos )
        count = sv.length( ) - index_str;
    insert(std::next(begin( ), index), sv.cbegin( ) + index_str, sv.cbegin( ) + index_str + count);
    return *this;
}

MY_STRING_CONSTEXPR void skl::string::resize(skl::string::size_type count)
{
    resize(count, '\0');
}

MY_STRING_CONSTEXPR void skl::string::resize(skl::string::size_type count, char ch)
{
    if ( count > max_size( ) )
        throw std::length_error("too long");

    if ( count < length( ) ) {
        tail  = begin( ) + count;
        *tail = '\0';
    } else if ( count > length( ) ) {
        append(count - length( ), ch);
    }
}

MY_STRING_CONSTEXPR skl::string& skl::string::replace(skl::string::size_type pos, skl::string::size_type count, const std::string& str, skl::string::size_type pos2, skl::string::size_type count2)
{
    std::string_view sv {str};
    return replace(pos, count, sv, pos2, count2);
}

MY_STRING_CONSTEXPR skl::string& skl::string::replace(skl::string::size_type pos, skl::string::size_type count, const std::string& str)
{
    std::string_view sv {str};
    return replace(pos, count, sv);
}

MY_STRING_CONSTEXPR skl::string& skl::string::replace(skl::string::const_iterator first, skl::string::const_iterator last, const std::string& str)
{
    std::string_view sv {str};
    return replace(first, last, sv);
}

MY_STRING_CONSTEXPR skl::string& skl::string::replace(skl::string::size_type pos, skl::string::size_type count, const char* cstr, skl::string::size_type count2)
{
    std::string_view sv {cstr};
    return replace(pos, count, sv, 0, count2);
}

MY_STRING_CONSTEXPR skl::string& skl::string::replace(skl::string::const_iterator first, skl::string::const_iterator last, const char* cstr, skl::string::size_type count2)
{
    std::string_view sv {cstr, count2};
    return replace(first, last, sv);
}

MY_STRING_CONSTEXPR skl::string& skl::string::replace(skl::string::size_type pos, skl::string::size_type count, const char* cstr)
{
    std::string_view sv {cstr};
    return replace(pos, count, sv);
}

MY_STRING_CONSTEXPR skl::string& skl::string::replace(skl::string::const_iterator first, skl::string::const_iterator last, const char* cstr)
{
    std::string_view sv {cstr};
    return replace(first, last, sv);
}

MY_STRING_CONSTEXPR skl::string& skl::string::replace(skl::string::const_iterator first, skl::string::const_iterator last, std::initializer_list<char> ilist)
{
    return replace(first, last, ilist.begin( ), ilist.end( ));
}

MY_STRING_CONSTEXPR skl::string& skl::string::replace(skl::string::const_iterator first, skl::string::const_iterator last, skl::string::size_type count2, char ch)
{
    skl::string::iterator iter = begin( ) + std::distance(cbegin( ), first);
    while ( iter != last && count2 > 0 ) {
        *iter = ch;
        iter++;
        count2--;
    }
    if ( count2 > 0 )
        erase(iter, iter + count2);
    return *this;
}

MY_STRING_CONSTEXPR skl::string& skl::string::replace(skl::string::size_type pos, skl::string::size_type count, skl::string::size_type count2, char ch)
{
    return replace(begin( ) + pos, begin( ) + pos + count, count2, ch);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find_last_not_of(const skl::string& str, skl::string::size_type pos) const noexcept
{
    return view( ).find_last_of(str.view( ), pos);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find_last_not_of(const std::string& str, skl::string::size_type pos) const noexcept
{
    return view( ).find_last_of(str, pos);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find_last_not_of(const char* s, skl::string::size_type pos, skl::string::size_type count) const
{
    return view( ).find_last_not_of(s, pos, count);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find_last_not_of(const char* s, skl::string::size_type pos) const
{
    return view( ).find_last_not_of(s, pos);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find_last_not_of(char ch, skl::string::size_type pos) const noexcept
{
    return view( ).find_last_not_of(ch, pos);
}

MY_STRING_CONSTEXPR skl::string::size_type skl::string::find_last_not_of(std::string_view sv, skl::string::size_type startPos) const noexcept
{
    return view( ).find_last_not_of(sv, startPos);
}

MY_STRING_CONSTEXPR void skl::string::swap(skl::string& other) noexcept
{
    size_t ourNewSize   = std::min(other.size( ), capacity( ));
    size_t theirNewSize = std::min(size( ), other.capacity( ));
    for ( size_t i = 0; i < std::min(max_size( ), other.max_size( )); i++ )
        std::swap(*(data( ) + i), *(other.data( ) + i));
    tail        = std::next(begin( ), ourNewSize);
    *tail       = '\0';
    other.tail  = std::next(other.begin( ), theirNewSize);
    *other.tail = '\0';
}

MY_STRING_CONSTEXPR std::string_view skl::string::substr(skl::string::size_type pos, skl::string::size_type count) const
{
    if ( pos > length( ) )
        throw std::out_of_range("index out of range");

    return view( ).substr(pos, count);
}

MY_STRING_CONSTEXPR std::string skl::string::std_str( ) const noexcept
{
    return std::string {view( )};
}
