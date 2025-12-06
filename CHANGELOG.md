### 0.6.0

- add more string manipulations functions in external header
- fix compilation warnings

### 0.5.0

- add `len_string` class for buffer + length pair strings
- full API for `static_string` implementation
- descriptions for `static_string` and for `len_string`

### 0.4.2

- add `static_string` constructor from `std::string_view`
- rename `wbr::str::trimWhitespaces` to `wbr::str::trim`
- rename `wbr::str::asHex` to `wbr::str::convertToHexString`
- change default match to partial match for `wbr::str::num` function

### 0.4.1

- add `system_handler.hxx` -- class name will be renamed soon to match naming schema in other files

### 0.4.0

- add `text_file_reader` class

### 0.3.1

- add `match_type_t` template paramter for `wbr::str::num` functions for full or partial match check

### 0.3

- `string_manipulations.hxx` utility functions added
