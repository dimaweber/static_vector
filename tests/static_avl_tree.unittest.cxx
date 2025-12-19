#include "static_avl_tree.hxx"

#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <gtest/gtest.h>

using namespace wbr;
using namespace testing;

struct RemoveTestDataset {
  std::vector<int> values;
  int              remkey;
};

std::ostream& operator<< (std::ostream& s, const RemoveTestDataset& d) {
  fmt::print(s, "data: {}  key: {}", d.values, d.remkey);
  return s;
}

std::vector<RemoveTestDataset> dataset {
    {{2},                                                 2 },
    {{2, 1},                                              1 },
    {{2, 1},                                              2 },
    {{2, 1, 3},                                           1 },
    {{2, 1, 3},                                           3 },
    {{2, 1, 3},                                           2 },
    {{7, 4, 12, 2, 5, 10, 14, 3, 6, 13, 15},              2 },
    {{7, 4, 12, 2, 5, 10, 14, 3, 6, 13, 15},              7 },
    {{7, 4, 12, 2, 5, 10, 14, 3, 6, 13, 15},              12},
    {{7, 4, 12, 2, 5, 10, 14, 3, 6, 13, 15},              15},
    {{7, 4, 12, 2, 5, 10, 14, 3, 6, 8, 11, 13, 15, 9},    7 },
    {{7, 4, 12, 2, 5, 10, 14, 3, 6, 11, 13, 15},          7 },
    {{7, 4, 12, 2, 5, 10, 14, 1, 3, 6, 8, 11, 13, 15, 9}, 13},
};

class AvlTree : public TestWithParam<RemoveTestDataset> { };

TEST_P (AvlTree, Remove) {
  auto [values, key] = GetParam( );
  static_avl_tree<int, int, 16> tree;
  for ( auto value: values )
    tree.add(value);

  tree.remove(key);
  EXPECT_TRUE(tree.valid( ));
}

INSTANTIATE_TEST_SUITE_P(Remove, AvlTree, ValuesIn(dataset));
