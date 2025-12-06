#pragma once
#include <concepts>
#include <string_view>

namespace wbr {
template<typename SV>
concept StringViewLike = std::is_convertible_v<const SV&, std::string_view> && !std::is_convertible_v<const SV&, const char*>;

template<typename I>
concept IndexLike = std::is_integral_v<I> && !std::is_pointer_v<I>;

}  // namespace wbr
