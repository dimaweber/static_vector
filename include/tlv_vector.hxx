#include <bit>
#include <iterator>
#include <span>

#include "bound_check.hxx"

namespace wbr {

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

        template<typename Value>
        [[nodiscard]] Value& value ( ) noexcept {
            return *std::bit_cast<Value*>(&v);
        }

        [[nodiscard]] std::span<std::byte> raw_value ( ) noexcept {
            return std::span<std::byte>(std::addressof(v[0]), length( ));
        }

        [[nodiscard]] std::span<const std::byte> raw_value ( ) const noexcept {
            return std::span<const std::byte>(std::addressof(v[0]), length( ));
        }

    private:
        Type      t;
        Length    l;
        std::byte v[1];
    };

    struct sentinel_t { };

    template<typename REC>
        requires std::is_same_v<typename std::remove_const<REC>::type, tlv_record>
    class tlv_iterator {
    public:
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

        [[nodiscard]] REC& operator* ( ) const {
            return *ptr_;
        }

        [[nodiscard]] REC* operator->( ) const {
            return ptr_;
        }

        [[nodiscard]] bool operator== (tlv_iterator o) const noexcept {
            return ptr_ == o.ptr_;
        }

        [[nodiscard]] std::strong_ordering operator<=> (tlv_iterator o) const noexcept {
            return ptr_ <=> o.ptr_;
        }

        [[nodiscard]] bool operator== (sentinel_t) const noexcept {
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

    explicit tlv_vector (void* data) noexcept : ptr_ {std::bit_cast<std::byte*>(data)} {
    }

    template<typename T, size_t SZ>
    explicit tlv_vector(std::array<T, SZ>& array) : ptr_ {std::bit_cast<std::byte*>(array.data( ))} {
    }

    [[nodiscard]] iterator begin ( ) noexcept {
        return iterator {std::bit_cast<tlv_record*>(ptr_)};
    }

    [[nodiscard]] const_iterator cbegin ( ) const noexcept {
        return const_iterator {std::bit_cast<const tlv_record*>(ptr_)};
    }

    [[nodiscard]] const_iterator begin ( ) const noexcept {
        return cbegin( );
    }

    [[nodiscard]] sentinel_t end ( ) noexcept {
        return sentinel_t { };
    }

    [[nodiscard]] sentinel_t end ( ) const noexcept {
        return sentinel_t { };
    }

    [[nodiscard]] sentinel_t cend ( ) const noexcept {
        return sentinel_t { };
    }

private:
    std::byte* ptr_ {nullptr};
};

}  // namespace wbr
