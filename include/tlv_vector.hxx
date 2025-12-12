#pragma once

#include <algorithm>
#include <bit>
#include <iterator>
#include <span>

#include "concepts.hxx"

namespace wbr {

template<std::integral T>
constexpr T htobe (T t) {
    if constexpr ( sizeof(T) == 1 ) {
        return t;
    } else if constexpr ( sizeof(T) == 2 ) {
        return static_cast<T>(htobe16(static_cast<uint16_t>(t)));
    } else if constexpr ( sizeof(T) == 4 ) {
        return static_cast<T>(htobe32(static_cast<uint32_t>(t)));
    } else if constexpr ( sizeof(T) == 8 ) {
        return static_cast<T>(htobe64(static_cast<uint64_t>(t)));
    } else {
        static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8, "Unsupported integral type size for htobe");
    }
    return 0;
}

template<std::integral T>
constexpr T betoh (T t) {
    if constexpr ( sizeof(T) == 1 ) {
        return t;
    } else if constexpr ( sizeof(T) == 2 ) {
        return static_cast<T>(be16toh(static_cast<uint16_t>(t)));
    } else if constexpr ( sizeof(T) == 4 ) {
        return static_cast<T>(be32toh(static_cast<uint32_t>(t)));
    } else if constexpr ( sizeof(T) == 8 ) {
        return static_cast<T>(be64toh(static_cast<uint64_t>(t)));
    } else {
        static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8, "Unsupported integral type size for betoh");
    }
    return 0;
}

template<std::integral Type = uint8_t, std::integral Length = uint8_t>
class tlv_vector {
public:
    class tlv_record {
    public:
        [[nodiscard]] Type type ( ) const noexcept {
            return t;
        }

        [[nodiscard]] Length length ( ) const noexcept {
            return l;
        }

        template<std::integral Value>
        [[nodiscard]] Value value ( ) const noexcept {
            Value val { };
            auto  ptr = std::bit_cast<std::byte*>(&val);
            std::copy_n(std::addressof(v[0]), sizeof(Value), ptr);
            return betoh(val);
        }

        template<StringViewLike Value>
        [[nodiscard]] Value value ( ) const noexcept {
            return Value {std::bit_cast<const char*>(std::addressof(v[0])), length( )};
        }

        template<typename Value>
        [[nodiscard]] Value value ( ) const noexcept {
            return *std::bit_cast<const Value*>(&v);
        }

        [[nodiscard]] std::span<std::byte> raw_value ( ) noexcept {
            return std::span<std::byte>(std::addressof(v[0]), length( ));
        }

        [[nodiscard]] std::span<const std::byte> raw_value ( ) const noexcept {
            return std::span<const std::byte>(std::addressof(v[0]), length( ));
        }

    private:
        void setType (Type newT) noexcept {
            t = newT;
        }

        void setLength (Length newL) noexcept {
            l = newL;
        }

        template<std::convertible_to<std::byte> DT>
        void setValue (std::span<DT> newV) noexcept {
            const auto begin = std::bit_cast<raw_pointer>(newV.data( ));
            const auto len   = newV.size( ) * sizeof(DT);
            std::copy_n(begin, len, std::addressof(v[0]));
        }

        template<std::integral DT>
        void setValue (DT newV) noexcept {
            auto ptr = std::bit_cast<raw_pointer>(&newV);
            std::copy_n(ptr, sizeof(DT), std::addressof(v[0]));
        }

        Type      t;
        Length    l;
        std::byte v[1];

        friend class tlv_vector;
    };

    template<typename REC>
        requires std::is_same_v<std::remove_const_t<REC>, tlv_record>
    class tlv_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = REC;
        using difference_type   = std::ptrdiff_t;
        using pointer           = REC*;
        using reference         = REC&;

        tlv_iterator& operator++ ( ) noexcept {
            std::byte* p = std::bit_cast<std::byte*>(ptr_);
            p += sizeof(Type) + sizeof(Length) + ptr_->length( );
            ptr_ = std::bit_cast<tlv_record*>(p);
            return *this;
        }

        tlv_iterator operator++ (int) noexcept {
            auto t = *this;
            operator++ ( );
            return t;
        }

        [[nodiscard]] reference operator* ( ) const {
            return *ptr_;
        }

        [[nodiscard]] pointer operator->( ) const {
            return ptr_;
        }

        [[nodiscard]] bool operator== (tlv_iterator o) const noexcept {
            return ptr_ == o.ptr_;
        }

        [[nodiscard]] std::strong_ordering operator<=> (tlv_iterator o) const noexcept {
            return ptr_ <=> o.ptr_;
        }

        [[nodiscard]] bool operator== (std::default_sentinel_t) const noexcept {
            return ptr_->type( ) == 0 && ptr_->length( ) == 0;
        }

    private:
        explicit tlv_iterator (REC* t) : ptr_ {t} {
        }

        REC* ptr_;
        friend class tlv_vector;
    };

    using iterator       = tlv_iterator<tlv_record>;
    using const_iterator = tlv_iterator<const tlv_record>;

    using raw_pointer = std::byte*;
    using rec_pointer = tlv_record*;

    explicit tlv_vector (void* data) noexcept : ptr_ {std::bit_cast<raw_pointer>(data)} {
    }

    template<typename T, size_t SZ>
    explicit tlv_vector(std::array<T, SZ>& array) : ptr_ {std::bit_cast<raw_pointer>(array.data( ))} {
    }

    [[nodiscard]] iterator begin ( ) noexcept {
        return iterator {std::bit_cast<rec_pointer>(ptr_)};
    }

    [[nodiscard]] const_iterator cbegin ( ) const noexcept {
        return const_iterator {std::bit_cast<const rec_pointer>(ptr_)};
    }

    [[nodiscard]] const_iterator begin ( ) const noexcept {
        return cbegin( );
    }

    [[nodiscard]] auto& end ( ) noexcept {
        return std::default_sentinel;
    }

    [[nodiscard]] auto& end ( ) const noexcept {
        return std::default_sentinel;
    }

    [[nodiscard]] auto& cend ( ) const noexcept {
        return std::default_sentinel;
    }

    template<typename T>
    iterator append (Type t, const T& data) {
        // raw_pointer cur = ptr_;
        // while ( *std::bit_cast<Type*>(cur) != 0 && *std::bit_cast<Length*>(cur + sizeof(Type)) != 0 )
        //     cur += sizeof(Type) + *std::bit_cast<Length*>(cur + sizeof(Type));
        iterator it = iterator_to_endElement( );

        it->setType(t);
        if constexpr ( wbr::StringViewLike<T> ) {
            it->setLength(data.length( ));
            it->setValue(std::span {std::bit_cast<raw_pointer>(data.data( )), data.length( )});
        } else if constexpr ( std::integral<T> ) {
            it->setLength(sizeof(T));
            it->setValue(htobe(data));
        } else {
            it->setLength(sizeof(data));
            it->setValue(std::span {std::bit_cast<raw_pointer>(&data), sizeof(data)});
        }

        auto e = it;
        ++e;
        e->setType(0);
        e->setLength(0);

        return it;
    }

    [[nodiscard]] iterator at (Type t) noexcept(noexcept(iterator_to_type(t))) {
        return iterator_to_type(t);
    }

private:
    iterator iterator_to_endElement ( ) noexcept {
        auto it = begin( );
        while ( it != end( ) ) {
            ++it;
        }
        return it;
    }

    iterator iterator_to_type (Type t) noexcept {
        auto it = begin( );
        while ( it != end( ) ) {
            if ( it->type( ) == t )
                break;
            ++it;
        }
        return it;
    }

    const raw_pointer ptr_ {nullptr};
};

}  // namespace wbr
