#pragma once
#include <concepts>
#include <string_view>

namespace wbr {
template<typename SV>
concept StringViewLike = std::is_convertible_v<const SV&, std::string_view> && !std::is_convertible_v<const SV&, const char*>;

template<typename I>
concept IndexLike = std::is_integral_v<I> && !std::is_pointer_v<I>;

template<typename T>
concept HasSubstr = requires(T t) { t.substr(0, 0); };

template<typename T>
concept HasBeginEnd = requires(T t) {
                        t.begin( );
                        t.end( );
                      };

template<typename T>
concept HasSize = requires(T t) {
                    t.size( );
                    t.empty( );
                  };

/** @brief Concept for containers with begin/end */
template<template<class> class V, class S>
concept ContainerLike = HasBeginEnd<V<S>>;

template<typename T>
concept IsVector = HasBeginEnd<T> and requires(T vec) {
                                        vec.push_back( );
                                        vec.at(size_t { });
                                      };

template<typename Ptr>
concept PointerLike = std::is_pointer_v<Ptr> || requires(Ptr p) {
                                                  { *p };
                                                  { static_cast<bool>(p) };
                                                  { p->operator->( ) } -> std::convertible_to<Ptr>;
                                                };

namespace str {
template<class S>
concept StringType = HasSize<S> and requires(S s) {
                                      s.find_first_of(' ', 0);
                                      s.data( );
                                    };

template<typename InputIt>
concept InputStrIt = requires(InputIt i) {
                       i++;
                       { *i } -> std::convertible_to<std::string_view>;
                     };

}  // namespace str

}  // namespace wbr
