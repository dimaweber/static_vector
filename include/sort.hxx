#pragma once
#include <fmt/format.h>

#include <algorithm>
#include <iterator>
#include <ranges>
#include <vector>

#include "concepts.hxx"

namespace wbr {
template<std::input_iterator Iter, std::input_iterator Iter2, auto cmp = std::less { }>
  requires requires(Iter it, Iter2 it2) {
             { *it == *it2 } -> std::convertible_to<bool>;
             { *it != *it2 } -> std::convertible_to<bool>;
             { cmp(*it, *it2) } -> std::convertible_to<bool>;
             std::iter_swap(it, it2);
           }
void sort_a(Iter b, Iter2 e) {
  if ( b == e )
    return;

  // size_t cmp_n {0};
  // size_t swap_n {0};
  for ( auto it = b; it != std::prev(e); ++it ) {
    for ( auto jt = std::next(it); jt != e; ++jt ) {
      // fmt::print("compare {} with {}", *it, *jt);
      // cmp_n++;
      if ( cmp(*jt, *it) ) {
        // swap_n++;
        //  fmt::println(" -> {} < {}, swap invoke", *jt, *it);
        std::iter_swap(it, jt);
      } else {
        // fmt::println(" no swap required, already ordered");
      }
    }
  }
  // fmt::println("sort_a {} elements done in {} compares and {} swaps", std::distance(b, e), cmp_n, swap_n);
}

auto sort_a (wbr::HasBeginEnd auto& vec) {
  return sort_a(vec.begin( ), vec.end( ));
}

auto sort_a (wbr::HasBeginEnd auto& vec, size_t n) {
  const auto r = std::min(std::distance(vec.begin( ), vec.end( )), n);
  return sort_a(vec.begin( ), vec.begin( ) + r);
}

template<std::input_iterator Iter, std::input_iterator Iter2, auto cmp = std::less { }>
  requires requires(Iter it, Iter2 it2) {
             { *it == *it2 } -> std::convertible_to<bool>;
             { *it != *it2 } -> std::convertible_to<bool>;
             { cmp(*it, *it2) } -> std::convertible_to<bool>;
           }
void sort_c(Iter b, Iter2 e) {
  if ( b == e )
    return;
  // size_t            cmp_n {0};
  const auto        n = std::distance(b, e);
  std::vector<uint> count(n, 0);
  for ( int i = 0; i < n - 1; ++i ) {
    for ( int j = i + 1; j < n; ++j ) {
      // cmp_n++;
      if ( cmp(*(b + i), *(b + j)) )
        ++count[j];
      else
        ++count[i];
    }
  }
  // fmt::println("sort_c {} elements done in {} compares", std::distance(b, e), cmp_n);
}

auto rotate_right (std::input_iterator auto i1, std::input_iterator auto i2, size_t step = 1) {
  size_t swap_n {0};
  for ( auto j = i2; j != i1; j -= step ) {
    std::iter_swap(j, std::prev(j, step));
    swap_n++;
  }
  return swap_n;
}

template<std::input_iterator Iter, std::input_iterator Iter2, auto cmp = std::less { }>
  requires requires(Iter it, Iter2 it2) {
             { *it == *it2 } -> std::convertible_to<bool>;
             { *it != *it2 } -> std::convertible_to<bool>;
             { cmp(*it, *it2) } -> std::convertible_to<bool>;
             std::iter_swap(it, it2);
           }
void sort_s(Iter b, Iter2 e) {
  if ( b == e )
    return;
  // size_t cmp_n {0};
  // size_t swap_n {0};
  for ( auto cur = std::next(b); cur != e; ++cur ) {
    auto ins = cur;
    for ( auto it = std::prev(cur); it != std::prev(b); --it ) {
      // cmp_n++;
      if ( cmp(*it, *cur) )
        break;
      ins = it;
    }
    /*swap_n +=*/rotate_right(ins, cur);
  }
  // fmt::println("sort_s {} elements done in {} compares and {} swaps", std::distance(b, e), cmp_n, swap_n);
}

template<std::input_iterator Iter, std::input_iterator Iter2, auto cmp = std::less { }>
  requires requires(Iter it, Iter2 it2) {
             { *it == *it2 } -> std::convertible_to<bool>;
             { *it != *it2 } -> std::convertible_to<bool>;
             { cmp(*it, *it2) } -> std::convertible_to<bool>;
             std::iter_swap(it, it2);
           }
void sort_s2(Iter b, Iter2 e) {
  if ( b == e )
    return;

  // size_t cmp_n {0};
  // size_t swap_n {0};
  for ( auto cur = 1; cur < std::distance(b, e); ++cur ) {
    auto p = 0;
    auto q = cur;
    while ( p < q ) {
      const auto t = (p + q) / 2;
      // cmp_n++;
      if ( cmp(*(b + cur), *(b + t)) )
        q = t;
      else
        p = t + 1;
    }
    /*swap_n += */ rotate_right(b + q, b + cur);
  }
  // fmt::println("sort_s2 {} elements done in {} compares and {} swaps", std::distance(b, e), cmp_n, swap_n);
}

const std::vector<long> shell_sort_ranges = {1, 4, 10, 23, 57, 132, 301, 701, 1750, 3937, 8858, 19930, 44842, 100894, 227011, 510774};

template<std::input_iterator Iter, std::input_iterator Iter2, auto cmp = std::less { }>
  requires requires(Iter it, Iter2 it2) {
             { *it == *it2 } -> std::convertible_to<bool>;
             { *it != *it2 } -> std::convertible_to<bool>;
             { cmp(*it, *it2) } -> std::convertible_to<bool>;
             std::iter_swap(it, it2);
           }
void sort_d(Iter b, Iter2 e) {
  if ( b == e )
    return;

  size_t cmp_n {0};
  size_t swap_n {0};

  const size_t n = std::distance(b, e);
  for ( const size_t h: std::views::reverse(shell_sort_ranges) ) {
    for ( size_t group = 0; group < h; ++group ) {
      for ( size_t cur = group + h; cur < n; cur += h ) {
        auto ins = cur;
        for ( ssize_t it = cur - h; it >= 0; it -= h ) {
          cmp_n++;
          if ( cmp(*(b + it), *(b + cur)) )
            break;
          ins = it;
        }
        swap_n += rotate_right(b + ins, b + cur, h);
      }
    }
  }
  fmt::println("sort_d {} elements done in {} compares and {} swaps", std::distance(b, e), cmp_n, swap_n);
}

template<std::input_iterator Iter, std::input_iterator Iter2, auto cmp = std::less { }>
  requires requires(Iter it, Iter2 it2) {
             { *it == *it2 } -> std::convertible_to<bool>;
             { *it != *it2 } -> std::convertible_to<bool>;
             { cmp(*it, *it2) } -> std::convertible_to<bool>;
             std::iter_swap(it, it2);
           }
void sort_d2(Iter b, Iter2 e) {
  if ( b == e )
    return;

  // size_t cmp_n {0};
  // size_t swap_n {0};

  const size_t n = std::distance(b, e);
  for ( const size_t h: std::views::reverse(shell_sort_ranges) ) {
    for ( size_t group = 0; group < h; ++group ) {
      for ( size_t cur = group + h; cur < n; cur += h ) {
        size_t p = 0;
        size_t q = cur / h;
        while ( p < q ) {
          const auto t = (p + q) / 2;
          // cmp_n++;
          if ( cmp(*(b + cur), *(b + group + t * h)) )
            q = t;
          else
            p = t + 1;
        }
        /*swap_n +=*/rotate_right(b + group + q * h, b + cur, h);
      }
    }
  }
  // fmt::println("sort_d2 {} elements done in {} compares and {} swaps", std::distance(b, e), cmp_n, swap_n);
}

template<std::input_iterator Iter, std::input_iterator Iter2, auto cmp = std::less { }>
  requires requires(Iter it, Iter2 it2) {
             { *it == *it2 } -> std::convertible_to<bool>;
             { *it != *it2 } -> std::convertible_to<bool>;
             { cmp(*it, *it2) } -> std::convertible_to<bool>;
             std::iter_swap(it, it2);
           }
void sort_d3(Iter b, Iter2 e) {
  if ( b == e )
    return;
  size_t cmp_n {0};
  size_t swap_n {0};

  for ( const auto d: std::views::reverse(shell_sort_ranges) ) {
    if ( d > std::distance(b, e) / 2 )
      continue;
    for ( auto i = b + d; i != e; ++i ) {
      for ( auto j = i; j - b >= d; j -= d ) {
        cmp_n++;
        if ( cmp(*j, *(j - d)) ) {
          std::iter_swap(j, j - d);
          swap_n++;
        } else {
          break;
        }
      }
    }
  }
  fmt::println("sort_d3 {} elements done in {} compares and {} swaps", std::distance(b, e), cmp_n, swap_n);
}
}  // namespace wbr
