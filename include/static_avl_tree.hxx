#pragma once
#include <array>
#include <concepts>
#include <functional>
#include <generator>
#include <numeric>
#include <queue>
#include <stack>
#include <unordered_set>

#include "concepts.hxx"
#include "static_vector.hxx"

namespace wbr {

constexpr uint log2 (std::unsigned_integral auto value) noexcept {
  return value == 0 ? 0 : sizeof(value) * CHAR_BIT - std::countl_zero(value) - 1;
}

template<typename T, IndexLike I, size_t storage_capacity = 256>
struct memory_t {
  std::array<T, storage_capacity>    backdata;
  static_vector<I, storage_capacity> list_of_empty_cells;
  static constexpr I                 invalid_index = static_cast<I>(-1);

  I allocate ( ) noexcept {
    if ( list_of_empty_cells.empty( ) )
      return invalid_index;

    return list_of_empty_cells.pop_back_value( );
  }

  void deallocate (I it) {
    list_of_empty_cells.push_back(it);
  }

  T& at (I index) noexcept {
    return backdata[index];
  }

  const T& at (I index) const noexcept {
    return backdata[index];
  }

  bool is_full ( ) const noexcept {
    return list_of_empty_cells.empty( );
  }

  memory_t ( ) {
    for ( I i = list_of_empty_cells.capacity( ); i > 0; --i ) {
      list_of_empty_cells.push_back(i - 1);
    }
  }

  constexpr size_t capacity ( ) const noexcept {
    return storage_capacity;
  }
};

/** @brief map based on stack-allocated vector
 *
 **/
template<std::totally_ordered Key, typename Value, size_t storage_capacity = 256>
  requires std::is_copy_assignable_v<Key> and std::is_copy_assignable_v<Value>
class static_avl_tree {
  using index_t                          = size_t;
  static constexpr index_t invalid_index = static_cast<index_t>(-1);

  struct record_t {
    Key     key;
    Value   value;
    index_t left {invalid_index};
    index_t right {invalid_index};
    index_t parent {invalid_index};

    ~record_t ( ) {
      key    = { };
      value  = { };
      left   = invalid_index;
      right  = invalid_index;
      parent = invalid_index;
    }
  };

  static_assert(IndexLike<index_t>);

  index_t head_ {invalid_index};

  consteval uint max_tree_depth (std::unsigned_integral auto value) noexcept {
    return log2(value) + 1;
  }

  int height (index_t idx) const noexcept {
    if ( idx == invalid_index )
      return -1;
    const auto& record = memory.at(idx);
    const int   lh {height(record.left)};
    const int   rh {height(record.right)};
    return std::max(lh, rh) + 1;
  }

  int balance_factor (index_t idx) const noexcept {
    if ( idx == invalid_index )
      return 0;
    const auto& record = memory.at(idx);
    return height(record.left) - height(record.right);
  }

  index_t rightRotate (index_t x) noexcept {
    const index_t y  = memory.at(x).left;
    const index_t T2 = memory.at(y).right;

    const auto parent_idx = memory.at(x).parent;
    if ( parent_idx != invalid_index ) {
      if ( memory.at(parent_idx).left == x ) {
        memory.at(parent_idx).left = y;
      } else {
        memory.at(parent_idx).right = y;
      }
    }

    memory.at(y).right  = x;
    memory.at(y).parent = memory.at(x).parent;

    memory.at(x).left   = T2;
    memory.at(x).parent = y;

    if ( T2 != invalid_index )
      memory.at(T2).parent = x;

    return y;
  }

  index_t leftRotate (index_t x) noexcept {
    const index_t y  = memory.at(x).right;
    const index_t T2 = memory.at(y).left;

    const auto parent_idx = memory.at(x).parent;
    if ( parent_idx != invalid_index ) {
      if ( memory.at(parent_idx).left == x ) {
        memory.at(parent_idx).left = y;
      } else {
        memory.at(parent_idx).right = y;
      }
    }

    memory.at(y).left   = x;
    memory.at(y).parent = memory.at(x).parent;

    memory.at(x).right  = T2;
    memory.at(x).parent = y;

    if ( T2 != invalid_index )
      memory.at(T2).parent = x;

    return y;
  }

  void lrc_action (index_t idx, std::function<void(const Key&, Value&)> action) noexcept(noexcept(action)) {
    if ( idx == invalid_index )
      return;
    record_t& rec = memory.at(idx);
    lrc_action(rec.left, action);
    lrc_action(rec.right, action);
    action(rec.key, rec.value);
  }

  void rlc_action (index_t idx, std::function<void(const Key&, Value&)> action) noexcept(noexcept(action)) {
    if ( idx == invalid_index )
      return;
    record_t& rec = memory.at(idx);
    rlc_action(rec.right, action);
    rlc_action(rec.left, action);
    action(rec.key, rec.value);
  }

  void clr_action (index_t idx, std::function<void(const Key&, Value&)> action) noexcept(noexcept(action)) {
    if ( idx == invalid_index )
      return;
    record_t& rec = memory.at(idx);
    action(rec.key, rec.value);
    clr_action(rec.left, action);
    clr_action(rec.right, action);
  }

  void lcr_action (index_t idx, std::function<void(const Key&, Value&)> action) noexcept(noexcept(action)) {
    if ( idx == invalid_index )
      return;
    record_t& rec = memory.at(idx);
    lcr_action(rec.left, action);
    action(rec.key, rec.value);
    lcr_action(rec.right, action);
  }

  void bfs_action (index_t idx, std::function<void(const Key&, Value&)> action) noexcept(noexcept(action)) {
    if ( idx == invalid_index )
      return;
    std::queue<index_t> queue;
    queue.push(idx);

    while ( !queue.empty( ) ) {
      index_t current_idx = queue.front( );
      queue.pop( );

      record_t& rec = memory.at(current_idx);
      action(rec.key, rec.value);

      if ( rec.left != invalid_index ) {
        queue.push(rec.left);
      }
      if ( rec.right != invalid_index ) {
        queue.push(rec.right);
      }
    }
  }

  std::generator<index_t> lcr_action_s (index_t start) {
    if ( start == invalid_index )
      co_return;
    std::stack<index_t, wbr::static_vector<index_t, max_tree_depth(storage_capacity)>> stack;

    while ( !stack.empty( ) || start != invalid_index ) {
      while ( start != invalid_index ) {
        stack.push(start);
        const auto& record = memory.at(start);

        if ( record.left != invalid_index ) {
          start = record.left;
        } else {
          break;
        }
      }
      if ( !stack.empty( ) ) {
        start = stack.top( );
        stack.pop( );
        auto& rec = memory.at(start);
        co_yield start;
        start = rec.right;
      } else
        start = invalid_index;
    }
  }

  std::generator<index_t> bfs_action_s (index_t start) {
    if ( start == invalid_index )
      co_return;
    std::queue<index_t> queue;
    queue.push(start);

    while ( !queue.empty( ) ) {
      index_t current_idx = queue.front( );
      queue.pop( );

      co_yield current_idx;

      const auto& rec = memory.at(current_idx);
      if ( rec.left != invalid_index ) {
        queue.push(rec.left);
      }
      if ( rec.right != invalid_index ) {
        queue.push(rec.right);
      }
    }
  }

  memory_t<record_t, index_t, storage_capacity> memory;

  index_t create (const Key& k, const Value& v, index_t parent = invalid_index) {
    const auto it  = memory.allocate( );
    record_t&  rec = *new (&memory.at(it)) record_t {k, v};
    rec.parent     = parent;
    return it;
  }

  index_t rebalance (index_t idx) {
    auto&     rec     = memory.at(idx);
    const int balance = balance_factor(idx);

    if ( balance > 1 ) {
      if ( balance_factor(rec.left) < 0 ) {
        rec.left = leftRotate(rec.left);
      }
      return rightRotate(idx);
    }

    if ( balance < -1 ) {
      if ( balance_factor(rec.right) > 0 ) {
        rec.right = rightRotate(rec.right);
      }
      return leftRotate(idx);
    }

    return idx;
  }

  std::pair<bool, index_t> new_node (const Key& k, const Value& v) {
    if ( memory.is_full( ) )
      return {false, invalid_index};

    if ( head_ == invalid_index ) {
      head_ = create(k, v);
      return {true, head_};
    }

    auto it = head_;
    while ( true ) {
      auto& parent = memory.at(it);
      if ( parent.key < k ) {
        if ( parent.right != invalid_index ) {
          it = parent.right;
        } else {
          parent.right = create(k, v, it);
          return {true, parent.right};
        }
      } else if ( parent.key > k ) {
        if ( parent.left != invalid_index ) {
          it = parent.left;
        } else {
          parent.left = create(k, v, it);
          return {true, parent.left};
        }
      } else {
        return {false, invalid_index};
      }
    }
  }

  bool validate_node_relationships (index_t idx) const {
    if ( idx == invalid_index )
      return true;

    const auto& node       = memory.at(idx);
    const auto  parent_idx = node.parent;

    // Check if parent pointers match with child relationships
    if ( parent_idx != invalid_index ) {
      const auto& parent        = memory.at(parent_idx);
      bool        is_left_child = false, is_right_child = false;
      if ( parent.left == idx )
        is_left_child = true;
      else if ( parent.right == idx )
        is_right_child = true;

      if ( !is_left_child && !is_right_child ) {
        return false;  // Parent-child relationship doesn't match
      }
    }

    // Check binary search tree property: left < parent, right > parent
    if ( (node.left != invalid_index && memory.at(node.left).key >= node.key) || (node.right != invalid_index && memory.at(node.right).key <= node.key) ) {
      return false;
    }

    // Recursively check all children
    return validate_node_relationships(node.left) && validate_node_relationships(node.right);
  }

  bool validate_balanced (index_t idx) const {
    return idx == invalid_index || (std::abs(balance_factor(idx)) < 2 && validate_balanced(memory.at(idx).left) && validate_balanced(memory.at(idx).right));
  }

  bool validate_no_loops (index_t current_idx) const {
    // Check if there are any loops in the tree
    std::unordered_set<index_t> visited;

    while ( current_idx != invalid_index ) {
      if ( visited.find(current_idx) != visited.end( ) ) {
        return false;  // Loop detected
      }
      visited.insert(current_idx);
      const auto& node = memory.at(current_idx);

      if ( node.left != invalid_index )
        current_idx = node.left;
      else if ( node.right != invalid_index )
        current_idx = node.right;
      else
        break;  // Reached a leaf, start backtracking to find other branches
    }
    return true;
  }

public:
  std::pair<bool, index_t> add (const Key& k, const Value& v) {
    auto [ok, new_node_idx] = new_node(k, v);
    if ( !ok )
      return {false, invalid_index};

    auto current_idx = new_node_idx;
    while ( current_idx != invalid_index && memory.at(current_idx).parent != invalid_index ) {
      const index_t parent_idx = memory.at(current_idx).parent;
      if ( parent_idx == head_ ) {
        head_ = rebalance(head_);
        break;
      }
      current_idx = rebalance(parent_idx);
    }

    return {true, new_node_idx};
  }

  std::pair<bool, index_t> add (const Key& k)
    requires std::same_as<Key, Value>
  {
    return add(k, k);
  }

  void lrc_action (std::function<void(const Key&, Value&)> action) {
    return lrc_action(head_, action);
  }

  void rlc_action (std::function<void(const Key&, Value&)> action) {
    return rlc_action(head_, action);
  }

  void clr_action (std::function<void(const Key&, Value&)> action) {
    return clr_action(head_, action);
  }

  void lcr_action (std::function<void(const Key&, Value&)> action) {
    return lcr_action(head_, action);
  }

  void lcr_action_s (std::function<void(const Key&, Value&)> action) {
    for ( const auto idx: lcr_action_s(head_) ) {
      auto& rec = memory.at(idx);
      action(rec.key, rec.value);
    }
  }

  void bfs_action (std::function<void(const Key&, Value&)> action) {
    return bfs_action(head_, action);
  }

  void bfs_action_s (std::function<void(const Key&, Value&)> action) {
    for ( const auto idx: bfs_action_s(head_) ) {
      auto& rec = memory.at(idx);
      action(rec.key, rec.value);
    }
  }

  bool validate_no_isolated_areas (index_t start_idx) const {
    if ( start_idx == invalid_index )
      return true;

    std::unordered_set<index_t> visited;
    std::stack<index_t>         to_visit;
    to_visit.push(start_idx);

    while ( !to_visit.empty( ) ) {
      index_t current_idx = to_visit.top( );
      to_visit.pop( );

      if ( visited.find(current_idx) != visited.end( ) ) {
        continue;  // Already visited
      }
      visited.insert(current_idx);

      const auto& node = memory.at(current_idx);
      if ( node.left != invalid_index )
        to_visit.push(node.left);
      if ( node.right != invalid_index )
        to_visit.push(node.right);
    }

    // Check if all nodes are visited
    for ( index_t i = 0; i < memory.capacity( ); ++i ) {
      const auto& node = memory.at(i);
      if ( (node.left != invalid_index || node.right != invalid_index || node.parent != invalid_index || i == head_) && visited.find(i) == visited.end( ) ) {
        return false;  // Found an isolated area
      }
    }

    return true;
  }

  bool valid ( ) const {
    // Check all nodes' relationships and binary search tree properties
    return validate_no_loops(head_) && validate_node_relationships(head_) && validate_no_isolated_areas(head_) && validate_balanced(head_);
  }

  index_t idx_by_key (const Key& k) const noexcept {
    auto idx = head_;
    while ( idx != invalid_index && memory.at(idx).key != k ) {
      if ( memory.at(idx).key < k )
        idx = memory.at(idx).right;
      else
        idx = memory.at(idx).left;
    }
    return idx;
  }

  Value& at_key (const Key& k) noexcept {
    const auto idx = idx_by_key(k);
    return memory.at(idx).value;
  }

  const Value& at_key (const Key& k) const noexcept {
    const auto idx = idx_by_key(k);
    return memory.at(idx).value;
  }

  bool remove (const Key& key) noexcept {
    auto idx = idx_by_key(key);
    if ( idx == invalid_index )
      return false;

    const record_t& rec             = memory.at(idx);
    const auto      parent_idx      = rec.parent;
    auto&           parent_link_ref = parent_idx == invalid_index ? head_ : (memory.at(parent_idx).left == idx ? memory.at(parent_idx).left : memory.at(parent_idx).right);
    auto            rebalance_idx   = invalid_index;
    if ( rec.left == invalid_index ) {
      parent_link_ref = rec.right;
      if ( rec.right != invalid_index )
        memory.at(rec.right).parent = parent_idx;
      rebalance_idx = parent_link_ref;
    } else if ( rec.right == invalid_index ) {
      parent_link_ref = rec.left;
      if ( rec.left != invalid_index )
        memory.at(rec.left).parent = parent_idx;
      rebalance_idx = parent_link_ref;
    } else {
      auto swap_idx = rec.right;
      while ( memory.at(swap_idx).left != invalid_index ) {
        swap_idx = memory.at(swap_idx).left;
      }
      record_t& swap_rec        = memory.at(swap_idx);
      auto      swap_parent_idx = swap_rec.parent;
      rebalance_idx             = swap_parent_idx;

      parent_link_ref = swap_idx;
      swap_rec.parent = parent_idx;
      if ( swap_parent_idx != invalid_index && swap_parent_idx != idx ) {
        memory.at(swap_parent_idx).left = swap_rec.right;
        if ( swap_rec.right != invalid_index )
          memory.at(swap_rec.right).parent = swap_parent_idx;
        swap_rec.right              = rec.right;
        memory.at(rec.right).parent = swap_idx;
        swap_rec.left               = rec.left;
        memory.at(rec.left).parent  = swap_idx;
      } else {
        swap_rec.left              = rec.left;
        memory.at(rec.left).parent = swap_idx;
        swap_rec.parent            = parent_idx;
      }
    }

    std::destroy_at(&rec);
    memory.deallocate(idx);

    if ( rebalance_idx == invalid_index ) {
      if ( head_ != invalid_index ) {
        head_ = rebalance(head_);
      }
    } else {
      do {
        if ( rebalance_idx == head_ ) {
          head_ = rebalance(rebalance_idx);
          break;
        }
        rebalance_idx = rebalance(rebalance_idx);
        rebalance_idx = memory.at(rebalance_idx).parent;
      } while ( rebalance_idx != invalid_index );
    }

    return true;
  }
};
}  // namespace wbr
