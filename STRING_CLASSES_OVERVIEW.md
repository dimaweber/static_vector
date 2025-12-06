# Static String Classes Overview

## Introduction

This document describes `static_string_adapter` and `static_string` classes from the `wbr` namespace, comparing them with C-strings and `std::string`.

---

## Class Descriptions

### 1. `static_string_adapter<BoundCheckStrategy>`

**Purpose:** A non-owning adapter that wraps an existing character buffer, providing a std::string-like interface without dynamic memory allocation.

**Key Characteristics:**
- **Non-owning:** Does not own the underlying buffer - you provide the buffer
- **Fixed capacity:** Cannot grow beyond the initial buffer size
- **No dynamic allocation:** All operations work within the provided buffer
- **Template parameter:** Configurable bound-checking strategy
- **Null-terminated:** Maintains null termination like C-strings
- **std::string-compatible API:** Most operations mirror `std::string`

**Internal Structure:**
```cpp
private:
    const pointer   head_;       // Pointer to buffer start
    pointer         tail_;       // Pointer to current end (null terminator)
    const size_type max_length_; // Maximum capacity (excluding null terminator)
```

**Memory Layout:**
```
Buffer:  [H][e][l][l][o][\0][?][?][?][?]
          ^                ^
        head_            tail_

capacity: 9 (buffer_size - 1)
size: 5 (distance from head_ to tail_)
```

**Usage Example:**
```cpp
char buffer[20];
wbr::static_string_adapter adapter(buffer, sizeof(buffer));
adapter.assign("Hello");
adapter.append(" World");
std::cout << adapter.view();  // "Hello World"
```

---

### 2. `static_string<SIZE, BoundCheckStrategy>`

**Purpose:** An owning string class with compile-time fixed capacity that manages its own buffer.

**Key Characteristics:**
- **Owning:** Owns its buffer (std::array member)
- **Compile-time capacity:** Size specified as template parameter
- **Inherits from:** `static_string_adapter` (composition + inheritance)
- **Self-contained:** No external buffer needed
- **Stack-allocated:** Entire object lives on stack

**Internal Structure:**
```cpp
class static_string : public static_string_adapter<bc_strategy> {
private:
    std::array<char, SZ> data_;  // Owned buffer
};
```

**Usage Example:**
```cpp
wbr::static_string<20> str;
str.assign("Hello");
str.append(" World");
std::cout << str.view();  // "Hello World"
```

---

## Bound Check Strategies

Both classes support four bound-checking strategies via template parameter:

| Strategy | Behavior on Overflow | Performance | Use Case |
|----------|---------------------|-------------|----------|
| `NoCheck` | Undefined behavior | Fastest | Release builds, trusted input |
| `Assert` | Assertion failure (debug only) | Fast in release | Development/testing |
| `Exception` | Throws exception | Slower | Need error recovery |
| `LimitToBound` | Silently truncates | Medium | Graceful degradation |

**Example:**
```cpp
wbr::static_string<10, BoundCheckStrategy::Exception> str;
try {
    str.assign("This is way too long");  // Throws std::overflow_error
} catch (const std::overflow_error& e) {
    // Handle error
}
```

---

## Feature Comparison

### Comparison Table

| Feature | C-String | `std::string` | `static_string_adapter` | `static_string` |
|---------|----------|---------------|------------------------|-----------------|
| **Memory Allocation** | Manual/Stack | Dynamic heap | User-provided buffer | Stack (std::array) |
| **Capacity** | Fixed at creation | Dynamic (grows) | Fixed at creation | Fixed at compile-time |
| **Null-terminated** | Yes | Yes (with c_str()) | Yes | Yes |
| **Length tracking** | O(n) with strlen() | O(1) | O(1) | O(1) |
| **Memory safety** | ❌ Unsafe | ✅ Safe | ⚠️ Configurable | ⚠️ Configurable |
| **Dynamic growth** | ❌ No | ✅ Yes | ❌ No | ❌ No |
| **STL algorithms** | Limited | ✅ Full support | ✅ Full support | ✅ Full support |
| **Exception safety** | N/A | Strong guarantee | Configurable | Configurable |
| **Constexpr support** | Limited | C++20+ | Most operations | Most operations |
| **Overhead** | None | Heap + bookkeeping | 2 pointers + size | Array + 2 ptrs + size |
| **Copy cost** | O(n) | O(n) + allocation | O(n) | O(n) |
| **Move cost** | N/A | O(1) | O(n)* | O(n)* |

*Move is same as copy because buffer cannot be transferred

---

## Detailed Comparisons

### vs. C-String (`char*` / `char[]`)

**Advantages over C-strings:**
1. ✅ **Length tracking:** O(1) `size()` vs O(n) `strlen()`
2. ✅ **Bounds checking:** Optional safety vs always unsafe
3. ✅ **Rich API:** Insert, erase, find, replace, etc. vs manual manipulation
4. ✅ **STL compatible:** Works with algorithms, ranges
5. ✅ **Type safety:** Clear string semantics vs raw pointers
6. ✅ **Automatic null-termination:** Managed automatically

**Advantages of C-strings:**
1. ✅ **Simplicity:** No class overhead
2. ✅ **C compatibility:** Direct use with C APIs
3. ✅ **Universal:** Works everywhere
4. ✅ **Zero overhead:** Minimal memory footprint

**Example Comparison:**
```cpp
// C-string approach
char buffer[20] = "Hello";
if (strlen(buffer) + 7 < sizeof(buffer)) {  // Manual checks
    strcat(buffer, " World");
}

// static_string_adapter approach
char buffer[20];
wbr::static_string_adapter<BoundCheckStrategy::Exception> str(buffer, 20);
str.assign("Hello");
str.append(" World");  // Automatic bounds checking
```

---

### vs. `std::string`

**Advantages over `std::string`:**
1. ✅ **No heap allocation:** Predictable memory usage
2. ✅ **Deterministic performance:** No allocation spikes
3. ✅ **Better cache locality:** Data on stack/inline
4. ✅ **Embedded-friendly:** Works without heap
5. ✅ **Real-time safe:** No allocator calls (small strings)
6. ✅ **Lower overhead:** No capacity management
7. ✅ **C API integration:** Direct buffer access

**Advantages of `std::string`:**
1. ✅ **Dynamic capacity:** Grows as needed
2. ✅ **No size limits:** Limited only by available memory
3. ✅ **Move semantics:** O(1) move operations
4. ✅ **SSO optimization:** Small strings optimized
5. ✅ **Standard:** Part of C++ standard library
6. ✅ **Universal support:** Available everywhere

**Example Comparison:**
```cpp
// std::string - dynamic allocation
std::string str1 = "Hello";
str1 += " World";  // May allocate
str1 += " from dynamic string";  // May allocate again

// static_string - fixed capacity
wbr::static_string<50> str2;
str2.assign("Hello");
str2.append(" World");  // No allocation
str2.append(" from static string");  // No allocation, guaranteed fits
```

---

## Performance Characteristics

### Time Complexity

| Operation | C-String | `std::string` | `static_string_adapter` | `static_string` |
|-----------|----------|---------------|------------------------|-----------------|
| Construction | O(1) | O(1) or O(n) | O(1) | O(1) |
| Assignment | O(n) | O(n) + alloc | O(n) | O(n) |
| Append | O(n) | O(1)* or O(n) | O(n) | O(n) |
| Insert | O(n) | O(n) + alloc | O(n) | O(n) |
| Erase | O(n) | O(n) | O(n) | O(n) |
| Find | O(n*m) | O(n*m) | O(n*m) | O(n*m) |
| Size/Length | O(n) | O(1) | O(1) | O(1) |
| Substring | O(n) | O(n) + alloc | O(1)** | O(1)** |
| Compare | O(n) | O(n) | O(n) | O(n) |

*Amortized constant if capacity available
**Returns string_view, no copy

### Space Complexity

```cpp
// C-string
char buffer[100];           // 100 bytes on stack

// std::string (typical implementation)
std::string str;            // 32 bytes object + heap allocation
                           // (24 bytes for pointer/size/capacity + 8 padding)

// static_string_adapter
char buffer[100];
static_string_adapter sa(buffer, 100);
                           // 24 bytes object (2 pointers + size_t)
                           // + 100 bytes user buffer

// static_string
static_string<100> ss;     // 124 bytes total on stack
                           // (100 byte array + 24 bytes adapter overhead)
```

---

## Use Cases

### When to Use `static_string_adapter`

✅ **Best for:**
- Wrapping existing buffers (stack, static, memory-mapped)
- Working with pre-allocated memory pools
- Interfacing with C APIs that provide buffers
- Zero-allocation requirements with external storage
- Temporary string operations on stack buffers

❌ **Avoid when:**
- Need dynamic growth
- Don't have a buffer to wrap
- Want simple value semantics

**Example:**
```cpp
void process_config(char* config_buffer, size_t buffer_size) {
    wbr::static_string_adapter adapter(config_buffer, buffer_size);
    adapter.assign("key=");
    adapter.append(get_config_value());
    // Work with rich API on existing buffer
}
```

---

### When to Use `static_string`

✅ **Best for:**
- Fixed-size strings with known maximum length
- Embedded systems without heap
- Real-time systems (predictable timing)
- Stack-based string processing
- Small to medium strings (<1KB typically)
- Performance-critical code paths
- Avoiding heap fragmentation

❌ **Avoid when:**
- String size unknown at compile time
- Need to handle arbitrarily large strings
- Dynamic growth is essential
- Memory constraints on stack

**Example:**
```cpp
wbr::static_string<256> error_message;
error_message.assign("Error at line ");
error_message.append(std::to_string(line_number));
error_message.append(": ");
error_message.append(error_description);
log(error_message.c_str());
```

---

### When to Use C-String

✅ **Best for:**
- Simple, short-lived operations
- Direct C API usage
- Literal strings
- Minimal memory overhead critical
- Legacy code integration

---

### When to Use `std::string`

✅ **Best for:**
- General-purpose string handling
- Unknown or varying string lengths
- Need dynamic growth
- Standard library integration
- Modern C++ codebases
- When heap allocation is acceptable

---

## API Overview

Both `static_string_adapter` and `static_string` provide a comprehensive API compatible with `std::string`:

### Capacity Operations
- `size()`, `length()` - Get current string length
- `capacity()`, `max_size()` - Get maximum capacity
- `empty()` - Check if empty
- `free_space()` - Get available space
- `resize(count)`, `resize(count, ch)` - Change size

### Element Access
- `operator[](pos)` - Unchecked access
- `at(pos)` - Checked access
- `front()`, `back()` - First/last character
- `c_str()`, `data()` - C-string pointer
- `view()` - Get std::string_view

### Modifiers
- `clear()` - Empty the string
- `assign(...)` - Replace contents (9 overloads)
- `append(...)` - Append to end (9 overloads)
- `insert(...)` - Insert at position (11 overloads)
- `erase(...)` - Remove characters (3 overloads)
- `replace(...)` - Replace substring (14 overloads)
- `push_back(ch)`, `pop_back()` - Add/remove at end
- `swap(other)` - Exchange contents
- `operator+=` - Append operators

### Search Operations
- `find(...)` - Find first occurrence (5 overloads)
- `find_first_of(...)` - Find first matching character (5 overloads)
- `find_first_not_of(...)` - Find first non-matching (5 overloads)
- `find_last_of(...)` - Find last matching character (5 overloads)
- `find_last_not_of(...)` - Find last non-matching (5 overloads)

### String Operations
- `compare(...)` - Lexicographical comparison (10 overloads)
- `starts_with(...)` - Check prefix (3 overloads)
- `ends_with(...)` - Check suffix (3 overloads)
- `contains(...)` - Check substring (3 overloads)
- `substr(pos, count)` - Extract substring
- `copy(dest, count, pos)` - Copy to buffer

### Comparison Operators
- `operator==(...)` - Equality comparison (3 overloads)
- `operator<=>(...)` - Three-way comparison (3 overloads)

### Iterators
- `begin()`, `end()` - Forward iterators
- `rbegin()`, `rend()` - Reverse iterators
- `cbegin()`, `cend()` - Const forward iterators
- `crbegin()`, `crend()` - Const reverse iterators

---

## Code Examples

### Example 1: Path Manipulation
```cpp
// Using static_string for file path
wbr::static_string<260, BoundCheckStrategy::LimitToBound> path;
path.assign("/home/user");
path.append("/documents");
path.append("/file.txt");

if (path.ends_with(".txt")) {
    process_text_file(path.c_str());
}
```

### Example 2: Protocol Message Building
```cpp
// Using static_string_adapter with pre-allocated buffer
char message_buffer[512];
wbr::static_string_adapter<BoundCheckStrategy::Exception> msg(
    message_buffer, sizeof(message_buffer));

msg.assign("GET /api/v1/");
msg.append(resource_name);
msg.append(" HTTP/1.1\r\n");
msg.append("Host: ");
msg.append(hostname);
msg.append("\r\n\r\n");

send_message(msg.data(), msg.size());
```

### Example 3: String Parsing
```cpp
char input[128] = "key1=value1;key2=value2";
wbr::static_string_adapter adapter(input, sizeof(input));

auto pos = adapter.find(';');
if (pos != adapter.npos) {
    auto first_pair = adapter.substr(0, pos);
    auto second_pair = adapter.substr(pos + 1);
    // Process pairs...
}
```

### Example 4: Real-Time Logging
```cpp
// No heap allocation, predictable performance
template<typename... Args>
void log_error(const char* format, Args... args) {
    wbr::static_string<512> log_msg;
    log_msg.assign("[ERROR] ");
    log_msg.append(format);
    // Format and send to log system
    send_to_logger(log_msg.c_str());
}
```

---

## Implementation Notes

### Memory Safety
- Bounds checking is configurable per-instance
- All operations respect capacity constraints
- Null termination is maintained automatically
- Buffer overruns are prevented (with appropriate strategy)

### Exception Safety
- Basic guarantee with `Exception` strategy
- No-throw guarantee with `NoCheck` strategy
- Strong guarantee for most read operations
- Weak guarantee for some write operations (capacity exceeded)

### Thread Safety
- Not thread-safe (like `std::string`)
- Concurrent reads are safe if no writes occur
- Use external synchronization for concurrent access

### Constexpr Support
- Most operations are `constexpr`
- Can be used in compile-time contexts (C++20+)
- Some operations (like `swap` with large strings) are not `constexpr`

---

## Conclusion

### Choose `static_string_adapter` when:
- You have an existing buffer to wrap
- Need zero-allocation string operations
- Interfacing with C APIs providing buffers
- Working with memory pools or custom allocators

### Choose `static_string` when:
- Need self-contained fixed-capacity string
- Embedded systems without heap
- Real-time performance requirements
- Stack-based string processing
- Maximum string size known at compile time

### Choose `std::string` when:
- Need dynamic capacity
- String sizes vary widely
- Heap allocation is acceptable
- General-purpose string handling

### Choose C-strings when:
- Simplest solution suffices
- Direct C API compatibility needed
- Minimal overhead is critical
- Legacy code integration

---

## Summary Table

| Criterion | C-String | `std::string` | `static_string_adapter` | `static_string` |
|-----------|----------|---------------|------------------------|-----------------|
| **Complexity** | Simple | Medium | Medium | Low |
| **Safety** | Low | High | Medium | Medium |
| **Performance** | Fast | Good | Fast | Fast |
| **Flexibility** | Low | High | Medium | Low |
| **Memory** | Stack | Heap | User-provided | Stack |
| **Best Use** | Simple ops | General use | Buffer wrapping | Fixed size |

