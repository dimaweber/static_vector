# Generator Template Comparison and Analysis

## Overview

This document compares `number_generator` and `token_generator`, analyzing their similarities and differences, and explains how they were unified into a single templated `generator<T, YieldT, ReturnT>`.

## Original Implementations

### number_generator
- **Storage Type**: `int currentValue`
- **Yield Parameter**: `int value`
- **Return Type**: `int`

### token_generator
- **Storage Type**: `std::string currentValue`
- **Yield Parameter**: `std::string_view value`
- **Return Type**: `std::string_view`

## Key Differences

The only structural differences are:
1. Type of `currentValue` in `promise_type`
2. Parameter type accepted by `yield_value()`
3. Return type of `value()` and iterator's `operator*()`

## Why Different Types?

For `token_generator`, we need three different types:

1. **Storage Type (`std::string`)**: We must **own** the string data because `co_yield` may receive a temporary or a view that won't survive between coroutine suspensions.

2. **Yield Type (`std::string_view`)**: Accepts lightweight views for efficiency. The `std::string` can be constructed from `std::string_view`.

3. **Return Type (`std::string_view`)**: Returns a non-owning view to avoid unnecessary copies when iterating.

For `number_generator`, all three types are the same (`int`) because integers are cheap to copy.

## Unified Template Solution

```cpp
template<typename T, typename YieldT = T, typename ReturnT = T>
    requires std::constructible_from<T, YieldT> && std::convertible_to<T, ReturnT>
struct generator;
```

### Template Parameters

- **T**: Storage type (what we store in `currentValue`)
- **YieldT**: Type accepted by `co_yield` (defaults to T)
- **ReturnT**: Type returned by iteration (defaults to T)

### Concept Constraints

1. **`std::constructible_from<T, YieldT>`**: Ensures we can construct storage type T from whatever is yielded (YieldT)
   - For strings: `std::string` can be constructed from `std::string_view` ✓

2. **`std::convertible_to<T, ReturnT>`**: Ensures the storage type can be converted to the return type
   - For strings: `std::string` implicitly converts to `std::string_view` ✓

## Usage Examples

### For integers (simple case):
```cpp
generator<int> generateNumbersGeneric(int start, int end) {
    for (int i = start; i <= end; ++i) {
        co_yield i;  // int → int → int
    }
}
```

### For strings (complex case):
```cpp
generator<std::string, std::string_view, std::string_view>
tokenizeEscapedGeneric(std::string_view input) {
    std::string currentToken;
    // ... parsing logic ...
    co_yield currentToken;  // std::string_view → std::string → std::string_view
}
```

## Required Traits for currentValue Type (T)

Based on the concept constraints and usage patterns:

1. **Default Constructible**: `T currentValue{};` requires default construction
2. **Constructible from YieldT**: `currentValue = value;` where `value` is YieldT
3. **Convertible to ReturnT**: Must be able to convert T to ReturnT for returning
4. **Move/Copy Constructible**: For moving/copying the generator (we allow move, delete copy)

### Explicit Constraints in Code:
- ✅ `std::constructible_from<T, YieldT>` - Can construct T from YieldT
- ✅ `std::convertible_to<T, ReturnT>` - Can convert T to ReturnT

### Implicit Requirements:
- ✅ Default constructible (from `T currentValue{}`)
- ✅ Move constructible (for generator's move operations)

## Benefits of Template Approach

1. **Code Reuse**: Single implementation instead of duplicating for each type
2. **Type Safety**: Compile-time checks via concepts
3. **Flexibility**: Supports both value types and view/storage type combinations
4. **Zero Overhead**: Template instantiation produces the same code as hand-written versions
5. **Clear Intent**: Template parameters document the storage/yield/return pattern

## Implementation Details

The template correctly handles:
- Coroutine lifecycle (construction, suspension, destruction)
- Move semantics (deleted copy, proper move)
- Iterator protocol for range-based for loops
- Proper suspend points (`initial_suspend`, `final_suspend`)

## Testing

Both implementations produce identical results:
- Original `number_generator`: generates 1-5
- Templated `generator<int>`: generates 10-14
- Original `token_generator`: tokenizes quoted strings
- Templated `generator<std::string, std::string_view, std::string_view>`: tokenizes quoted strings

All tests passed successfully! ✓

## C++23 Standard Library Support

### `std::generator<T>` (C++23)

Starting with **C++23**, the standard library provides `<generator>` header with `std::generator<T>`:

```cpp
#include <generator>

std::generator<int> generateNumbers(int start, int end) {
    for (int i = start; i <= end; ++i) {
        co_yield i;
    }
}

// Usage
for (int i : generateNumbers(1, 5)) {
    std::cout << i << '\n';
}
```

### Key Differences from Our Template

| Feature | Our `generator<T, YieldT, ReturnT>` | C++23 `std::generator<T>` |
|---------|-------------------------------------|---------------------------|
| **Template Parameters** | 3 (Storage, Yield, Return) | 1 (Yielded type) |
| **Flexibility** | Separate storage/yield/return types | Single type (or reference) |
| **Complexity** | More complex, explicit control | Simpler, handles common cases |
| **String Views** | Can yield string_view, store string, return string_view | Must yield string (ownership required) |

### When to Use What

**Use `std::generator<T>` (C++23+) when:**
- ✅ You have C++23 compiler support
- ✅ Simple cases where yielded type = storage type
- ✅ Standard, well-tested implementation
- ✅ Better optimization potential (compiler-specific)
- ✅ Simpler code, less boilerplate

**Use custom `generator<T, YieldT, ReturnT>` when:**
- ✅ Pre-C++23 codebase
- ✅ Need explicit control over storage/yield/return types
- ✅ Educational purposes (understanding coroutines)
- ✅ Very specific optimization needs

### C++23 String Tokenizer Example

With `std::generator`, you **must** yield owned strings, not views:

```cpp
// CORRECT: Yields owned strings
std::generator<std::string> tokenize(std::string_view input) {
    std::string token;
    // ... parsing ...
    co_yield token;  // Copies/moves the string
}

// WRONG: Would create dangling string_views
std::generator<std::string_view> tokenize(std::string_view input) {
    std::string token;
    co_yield token;  // ❌ Returns view to local variable!
}
```

Our custom template allows the three-type pattern:
```cpp
generator<std::string, std::string_view, std::string_view>
```
to store strings, accept views for efficiency, and return views without copies.

### Compiler Support Status

| Compiler | Version | `<generator>` Support |
|----------|---------|----------------------|
| **GCC** | 14+ | ✅ Yes |
| **Clang** | 18+ | ✅ Yes (with libc++) |
| **MSVC** | 19.35+ (VS 2022 17.5+) | ✅ Yes |

### Other C++23/26 Coroutine Features

**C++23 also added:**
- `std::generator` - Lazy sequence generation (✅ demonstrated above)

**C++26 may add:**
- `std::task` - Async task type (proposal P2300)
- `std::async_generator` - Async lazy sequences (proposal P2502)
- Better coroutine debugging support

**Future (C++29+) possibilities:**
- More coroutine utilities in `<ranges>` integration
- Stackless async primitives
- Better error handling for coroutines

### Migration Strategy

If targeting C++23+:
1. Use `std::generator<T>` for new code
2. Keep custom generators for:
   - Pre-C++23 compatibility
   - Special storage/yield/return patterns
   - Educational examples

Our demonstration shows all three approaches working side-by-side! ✓
