# len_string_adapter - Buffer+Length String Adapter

## Overview

`len_string_adapter` is designed for the common embedded systems and communication protocol pattern where strings are stored as:
- A fixed-size character buffer
- A separate length field (typically `uint8_t`, `uint16_t`, etc.)

This is in contrast to:
- **C-strings**: null-terminated
- **static_string_adapter**: null-terminated with tracked length
- **len_string_adapter**: length-only (no null termination required)

## Key Features

- ✅ **No null-termination overhead** - Uses only the space needed for characters
- ✅ **std::string-like API** - Familiar interface for string operations
- ✅ **Maintains external length** - Updates your length field automatically
- ✅ **Optional null-termination** - Can add null terminator via `c_str()` if space available
- ✅ **Multiple construction options** - From buffer+length, arrays, spans, iterators
- ✅ **Configurable length type** - `uint8_t`, `uint16_t`, `size_t`, etc.
- ✅ **Bound-checking strategies** - Same as `static_string_adapter`
- ✅ **Constexpr support** - Most operations are `constexpr`

---

## Basic Usage

### Example 1: Protocol Message Structure

```cpp
#include "static_string.hxx"
#include <cstdint>

struct ChatMessage {
    uint64_t sender_id;
    uint64_t receiver_id;
    char     message[256];
    uint8_t  len;  // Current length of message
};

void send_chat_message() {
    ChatMessage msg{};
    msg.sender_id = 12345;
    msg.receiver_id = 67890;

    // Create adapter for the message field
    wbr::len_string_adapter<uint8_t> adapter(msg.message, 256, msg.len);

    adapter.assign("Hello, ");
    adapter.append("how are you?");

    // msg.len is automatically updated to 18
    // msg.message contains "Hello, how are you?" (no null terminator needed)

    send_over_network(&msg, sizeof(msg));
}
```

### Example 2: Network Packet

```cpp
struct NetworkPacket {
    uint16_t packet_id;
    uint16_t sequence;
    char     payload[1024];
    uint16_t payload_len;
};

void build_packet(uint16_t id, uint16_t seq, std::string_view data) {
    NetworkPacket packet{};
    packet.packet_id = id;
    packet.sequence = seq;

    wbr::len_string_adapter<uint16_t> adapter(packet.payload, 1024, packet.payload_len);
    adapter.assign(data);

    // packet.payload_len is automatically set to data.size()
    transmit_packet(&packet);
}
```

---

## Construction Options

### 1. From Buffer Pointer and Capacity

```cpp
char buffer[100];
uint8_t len = 0;
wbr::len_string_adapter<uint8_t> adapter(buffer, 100, len);
```

### 2. From std::array

```cpp
std::array<char, 100> arr;
uint8_t len = 0;
wbr::len_string_adapter<uint8_t> adapter(arr, len);
```

### 3. From C-style Array

```cpp
char buffer[100];
uint8_t len = 0;
wbr::len_string_adapter<uint8_t> adapter(buffer, len);  // Size deduced
```

### 4. From std::span

```cpp
char buffer[100];
uint8_t len = 0;
std::span<char> sp{buffer, 100};
wbr::len_string_adapter<uint8_t> adapter(sp, len);
```

### 5. From Iterators

```cpp
std::vector<char> vec(100);
uint8_t len = 0;
wbr::len_string_adapter<uint8_t> adapter(vec.begin(), vec.end(), len);
```

---

## Length Types

The adapter is templated on the length type, allowing you to match your protocol's length field:

```cpp
// For small buffers (<256 bytes)
uint8_t len8 = 0;
wbr::len_string_adapter<uint8_t> small_adapter(buffer, 200, len8);

// For medium buffers (<65536 bytes)
uint16_t len16 = 0;
wbr::len_string_adapter<uint16_t> medium_adapter(buffer, 10000, len16);

// For large buffers
size_t len_size = 0;
wbr::len_string_adapter<size_t> large_adapter(buffer, 100000, len_size);
```

**Important:** The effective capacity is `min(buffer_capacity, max_value_of_LenType)`

```cpp
char buffer[300];
uint8_t len = 0;
wbr::len_string_adapter<uint8_t> adapter(buffer, 300, len);

// capacity() returns 255, not 300!
assert(adapter.capacity() == 255);
```

---

## No Automatic Null-Termination

Unlike `static_string_adapter`, `len_string_adapter` does **not** automatically null-terminate strings:

```cpp
char buffer[10];
uint8_t len = 0;
wbr::len_string_adapter<uint8_t> adapter(buffer, 10, len);

adapter.assign("Hello");
// len == 5
// buffer contains: ['H', 'e', 'l', 'l', 'o', ?, ?, ?, ?, ?]
// No '\0' written!
```

### Getting Null-Terminated String

Use `c_str()` to add null termination **if space is available**:

```cpp
char buffer[10];
uint8_t len = 0;
wbr::len_string_adapter<uint8_t> adapter(buffer, 10, len);

adapter.assign("Hello");

// c_str() adds '\0' if there's space
const char* cstr = adapter.c_str();
if (cstr != nullptr) {
    printf("%s\n", cstr);  // Works! Buffer now has '\0' at position 5
}
```

If buffer is full, behavior depends on bound-check strategy:
```cpp
char buffer[5];
uint8_t len = 0;
wbr::len_string_adapter<uint8_t> adapter(buffer, 5, len);
adapter.assign("12345");  // Fills entire buffer

// NoCheck strategy: returns nullptr
const char* cstr1 = adapter.c_str();
assert(cstr1 == nullptr);

// Exception strategy: throws
wbr::len_string_adapter<uint8_t, BoundCheckStrategy::Exception>
    adapter2(buffer, 5, len);
// throws std::length_error
// const char* cstr2 = adapter2.c_str();
```

---

## API Reference

### Capacity

```cpp
size_type size() const noexcept;         // Current length
size_type length() const noexcept;       // Same as size()
size_type capacity() const noexcept;     // Maximum capacity
size_type max_size() const noexcept;     // Same as capacity()
bool empty() const noexcept;             // Is length == 0?
size_type free_space() const noexcept;   // Remaining space
```

### Element Access

```cpp
reference operator[](size_type pos);           // Unchecked access
const_reference operator[](size_type pos) const;

reference at(size_type pos);                    // Checked access (throws)
const_reference at(size_type pos) const;

reference front();                              // First character
const_reference front() const;

reference back();                               // Last character
const_reference back() const;

pointer data() noexcept;                        // Buffer pointer
const_pointer data() const noexcept;

std::string_view view() const noexcept;         // Get string_view
operator std::string_view() const noexcept;     // Implicit conversion

const_pointer c_str() const;                    // Null-terminated (if space)
```

### Modifiers

```cpp
void clear() noexcept;                          // Set length to 0

len_string_adapter& assign(std::string_view sv);
len_string_adapter& assign(const char* s);
len_string_adapter& assign(size_type count, char ch);

len_string_adapter& append(std::string_view sv);
len_string_adapter& append(const char* s);
len_string_adapter& append(size_type count, char ch);

len_string_adapter& operator+=(std::string_view sv);
len_string_adapter& operator+=(const char* s);
len_string_adapter& operator+=(char ch);

void push_back(char ch);
void pop_back();

void resize(size_type count);
void resize(size_type count, char ch);
```

### Iterators

```cpp
iterator begin() noexcept;
const_iterator begin() const noexcept;
const_iterator cbegin() const noexcept;

iterator end() noexcept;
const_iterator end() const noexcept;
const_iterator cend() const noexcept;

reverse_iterator rbegin() noexcept;
const_reverse_iterator rbegin() const noexcept;
const_reverse_iterator crbegin() const noexcept;

reverse_iterator rend() noexcept;
const_reverse_iterator rend() const noexcept;
const_reverse_iterator crend() const noexcept;
```

### Comparison

```cpp
bool operator==(std::string_view sv) const noexcept;
bool operator==(const char* s) const noexcept;

int operator<=>(std::string_view sv) const noexcept;
int operator<=>(const char* s) const noexcept;
```

### String Operations

```cpp
std::string_view substr(size_type pos = 0, size_type count = npos) const;
size_type copy(char* dest, size_type count, size_type pos = 0) const;
int compare(std::string_view sv) const noexcept;

bool starts_with(std::string_view sv) const noexcept;
bool starts_with(char ch) const noexcept;

bool ends_with(std::string_view sv) const noexcept;
bool ends_with(char ch) const noexcept;

bool contains(std::string_view sv) const noexcept;
bool contains(char ch) const noexcept;

size_type find(std::string_view sv, size_type pos = 0) const noexcept;
size_type find(char ch, size_type pos = 0) const noexcept;
```

---

## Bound-Check Strategies

All modifying operations support bound-check strategy overrides:

```cpp
// NoCheck (default): No checking, fastest
wbr::len_string_adapter<uint8_t> adapter1(buffer, 10, len);
adapter1.append("Too long string");  // May overflow!

// Assert: Check in debug builds only
wbr::len_string_adapter<uint8_t, BoundCheckStrategy::Assert> adapter2(buffer, 10, len);
adapter2.append("Too long");  // Asserts in debug, UB in release

// Exception: Always check, throw on overflow
wbr::len_string_adapter<uint8_t, BoundCheckStrategy::Exception> adapter3(buffer, 10, len);
try {
    adapter3.append("Too long string");  // Throws std::overflow_error
} catch (const std::overflow_error& e) {
    // Handle error
}

// LimitToBound: Silently truncate to fit
wbr::len_string_adapter<uint8_t, BoundCheckStrategy::LimitToBound> adapter4(buffer, 10, len);
adapter4.append("Too long string");  // Writes "Too long s", len=10
```

### Per-Operation Strategy Override

```cpp
wbr::len_string_adapter<uint8_t> adapter(buffer, 10, len);

// Use different strategy for specific operation
adapter.append<BoundCheckStrategy::Exception>("data");
```

---

## Real-World Examples

### Example 1: UART Protocol

```cpp
struct UARTFrame {
    uint8_t  start_byte;     // 0xAA
    uint8_t  command;
    char     data[128];
    uint8_t  data_len;
    uint8_t  checksum;
    uint8_t  end_byte;       // 0x55
};

void send_uart_command(uint8_t cmd, std::string_view data) {
    UARTFrame frame{};
    frame.start_byte = 0xAA;
    frame.command = cmd;
    frame.end_byte = 0x55;

    wbr::len_string_adapter<uint8_t, BoundCheckStrategy::LimitToBound>
        adapter(frame.data, 128, frame.data_len);

    adapter.assign(data);  // Truncates if too long

    // Calculate checksum over data
    frame.checksum = calculate_checksum(frame.data, frame.data_len);

    uart_transmit(&frame, sizeof(frame));
}
```

### Example 2: Binary Protocol with Header

```cpp
#pragma pack(push, 1)
struct BinaryMessage {
    uint32_t magic;          // 0xDEADBEEF
    uint16_t version;
    uint16_t msg_type;
    char     payload[512];
    uint16_t payload_size;
    uint32_t crc32;
};
#pragma pack(pop)

void serialize_message(uint16_t type, std::string_view content) {
    BinaryMessage msg{};
    msg.magic = 0xDEADBEEF;
    msg.version = 1;
    msg.msg_type = type;

    wbr::len_string_adapter<uint16_t> adapter(msg.payload, 512, msg.payload_size);
    adapter.assign(content);

    msg.crc32 = calculate_crc32(&msg,
        offsetof(BinaryMessage, payload) + msg.payload_size);

    write_to_stream(&msg,
        offsetof(BinaryMessage, crc32) + sizeof(msg.crc32));
}
```

### Example 3: JSON String Builder (No Heap Allocation)

```cpp
struct JSONBuilder {
    char data[1024];
    uint16_t len;

    wbr::len_string_adapter<uint16_t, BoundCheckStrategy::LimitToBound>
        adapter;

    JSONBuilder() : len(0), adapter(data, 1024, len) {
        adapter.assign("{");
    }

    void add_field(std::string_view key, std::string_view value) {
        if (len > 1) adapter.append(",");  // Not first field
        adapter.append("\"");
        adapter.append(key);
        adapter.append("\":\"");
        adapter.append(value);
        adapter.append("\"");
    }

    void add_number(std::string_view key, int value) {
        if (len > 1) adapter.append(",");
        adapter.append("\"");
        adapter.append(key);
        adapter.append("\":");
        adapter.append(std::to_string(value));
    }

    std::string_view finalize() {
        adapter.append("}");
        return adapter.view();
    }
};

// Usage
JSONBuilder json;
json.add_field("name", "Alice");
json.add_number("age", 30);
json.add_field("city", "NYC");
auto result = json.finalize();
// result: {"name":"Alice","age":30,"city":"NYC"}
```

### Example 4: Embedded System Log Buffer

```cpp
struct LogEntry {
    uint32_t timestamp;
    uint8_t  level;      // ERROR, WARNING, INFO, DEBUG
    char     message[256];
    uint8_t  msg_len;
};

class Logger {
    LogEntry ring_buffer[100];
    size_t write_pos = 0;

public:
    void log(uint8_t level, std::string_view msg) {
        LogEntry& entry = ring_buffer[write_pos % 100];
        entry.timestamp = get_system_time();
        entry.level = level;

        wbr::len_string_adapter<uint8_t, BoundCheckStrategy::LimitToBound>
            adapter(entry.message, 256, entry.msg_len);

        adapter.assign(msg);  // Truncates if too long

        write_pos++;
    }
};
```

---

## Comparison with Other String Types

| Feature | C-String | `std::string` | `static_string_adapter` | `len_string_adapter` |
|---------|----------|---------------|------------------------|---------------------|
| Storage | Buffer | Heap | Buffer (null-term) | Buffer + length var |
| Length tracking | O(n) | O(1) | O(1) | O(1) |
| Null-terminated | Always | Via c_str() | Always | Via c_str() if space |
| Memory overhead | 1 byte (\0) | 24+ bytes | 1 byte (\0) | 0 bytes |
| Max efficiency | Length-1 | Variable | Length-1 | Length |
| Length variable | None | Internal | Internal | External (yours) |
| Protocol friendly | Medium | Poor | Good | Excellent |

---

## Best Practices

### 1. Choose Appropriate Length Type
```cpp
// Bad: uint8_t with 1000-byte buffer
char buffer[1000];
uint8_t len = 0;
wbr::len_string_adapter<uint8_t> bad(buffer, 1000, len);
// capacity() == 255, not 1000!

// Good: Match length type to buffer size
uint16_t len16 = 0;
wbr::len_string_adapter<uint16_t> good(buffer, 1000, len16);
// capacity() == 1000
```

### 2. Use Bound Checking in Development
```cpp
#ifdef DEBUG
    wbr::len_string_adapter<uint8_t, BoundCheckStrategy::Assert> adapter(buf, sz, len);
#else
    wbr::len_string_adapter<uint8_t> adapter(buf, sz, len);
#endif
```

### 3. Prefer view() Over c_str()
```cpp
// Good: No null termination needed
auto view = adapter.view();
process_data(view.data(), view.size());

// Less efficient: May need to add '\0'
const char* cstr = adapter.c_str();
if (cstr) legacy_function(cstr);
```

### 4. Use LimitToBound for User Input
```cpp
wbr::len_string_adapter<uint8_t, BoundCheckStrategy::LimitToBound>
    adapter(msg.data, 256, msg.len);

// Safely handle potentially oversized input
adapter.assign(user_input);  // Auto-truncates if needed
```

---

## Summary

`len_string_adapter` is ideal for:
- ✅ Communication protocols with buffer+length structures
- ✅ Embedded systems wanting maximum space efficiency
- ✅ Binary data formats
- ✅ Fixed-size message structures
- ✅ Zero-overhead string operations
- ✅ Interfacing with external length variables

Choose `len_string_adapter` over `static_string_adapter` when:
- You don't need null termination
- You already have a length field in your structure
- You want to maximize usable buffer space
- You're working with binary protocols
