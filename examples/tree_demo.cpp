#include <fmt/format.h>

#include "loremipsum.hxx"
#include "static_avl_tree.hxx"

void tree_demo ( ) {
  wbr::static_avl_tree<int, std::string, 512> map;

  map.add(4, "four");
  map.add(5, "five");
  map.add(6, "six");
  map.add(9, "nine");
  map.add(2, "two");
  map.add(3, "three");
  map.clr_action([] (int i, std::string& s) { fmt::println("{} {}", i, s); });
  map.add(8, "eight");
  map.add(7, "seven");
  map.add(1, "one");
  map.add(0, "zero");
  fmt::println("tree valid: {}", map.valid( ));

  fmt::println("right - left - center");
  map.rlc_action([] (int i, std::string& s) { fmt::println("{} {}", i, s); });
  fmt::println("left - right - center");
  map.lrc_action([] (int i, std::string& s) { fmt::println("{} {}", i, s); });
  fmt::println("center - left - right");
  map.clr_action([] (int i, std::string& s) { fmt::println("{} {}", i, s); });
  fmt::println("left - center - right");
  map.lcr_action([] (int i, std::string& s) { fmt::println("{} {}", i, s); });
  fmt::println("left - center - right, non-recursive");
  map.lcr_action_s([] (int i, std::string& s) { fmt::println("{} {}", i, s); });

  wbr::lorem::loremipsum_t gen;
  wbr::lorem::composer_t   comp {gen};
  for ( int i = 10; i < 610; ++i ) {
    auto [ok, idx] = map.add(i, fmt::format("word {}", gen.next( )));
    if ( !ok ) {
      fmt::println("failed to add key {}", i);
    }
  }
  map.lcr_action_s([] (int i, std::string& s) { fmt::println("{} {}", i, s); });
  fmt::println("tree valid: {}", map.valid( ));

  for ( int in: {100, 200, 400, 300, 500, 0, 609} ) {
    map.remove(in);
    fmt::println("after removing key {}: {}", in, map.valid( ));
  }

  wbr::static_avl_tree<int, int, 16> tree;
  tree.add(7, 7);
  tree.add(4, 4);
  tree.add(12, 12);
  tree.add(3, 3);
  tree.add(5, 5);
  tree.add(10, 10);
  tree.add(14, 14);
  tree.add(6, 6);
  tree.add(13, 13);
  tree.add(15, 15);
  fmt::println("tree valid: {}, size: {}", tree.valid( ), tree.size( ));
  tree.remove(7);
  fmt::println("after removing key {}, size: {}", tree.valid( ), tree.size( ));

  tree.bfs_action_s([] (const int& i, int& s) { fmt::println("{} {}", i, s); });

  fmt::println("value at {} is {}", 6, tree.at(6));
  fmt::println("value at {} is {}", 10, tree[10]);

  try {
    fmt::println("value at {} is {}", 42, tree.at(42));
  } catch ( const std::out_of_range& e ) {
    fmt::println("caught exception: {}", e.what( ));
  }

  for ( auto [key, value]: tree )
    fmt::println("{} {}", key, value);

  fmt::println("tree count: {} vs std::count {}", tree.size( ), std::count_if(tree.begin( ), tree.end( ), [] (auto) { return true; }));

  tree.clear( );
  fmt::println("after clearing tree valid: {}, size: {}", tree.valid( ), tree.size( ));

  return;
}
