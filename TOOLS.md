Debugger Tools: LLDB Python pretty-printers

This project includes LLDB Python pretty-printers to improve how the following types display in CLion/LLDB:

- wbr::static_string<...>
- wbr::static_string_adapter<...>
- wbr::static_vector<...>
- wbr::static_vector_adapter<...>

They live in the tools/ directory and register themselves automatically when imported by LLDB.

- tools/pretty_static_string.py — summary provider for static strings and adapters.
- tools/pretty_static_vector.py — summary + synthetic children for static vectors and adapters.

Setup via ~/.lldbinit (recommended):
- Add absolute paths:
  - command script import /home/weber/projects/static_vector/tools/pretty_static_string.py
  - command script import /home/weber/projects/static_vector/tools/pretty_static_vector.py
- In CLion: enable “Use ~/.lldbinit” and “Use Python renderers” in LLDB settings.

Per-session import (LLDB console):
- (lldb) command script import /home/weber/projects/static_vector/tools/pretty_static_string.py
- (lldb) command script import /home/weber/projects/static_vector/tools/pretty_static_vector.py

What you’ll see:
- static_vector: static_vector(size=N, cap=M) [e0, e1, e2, …]; children are elements [0..N-1].
- static_vector_adapter: static_vector_adapter(size=N, cap=M) [e0, e1, e2, …].
- static_string: "..." len=X cap=Y.

Verification:
- (lldb) type summary list | grep wbr::static_vector
- (lldb) type synthetic list | grep wbr::static_vector
- (lldb) type summary list | grep static_vector_adapter
- (lldb) type synthetic list | grep static_vector_adapter

Troubleshooting:
- If printers don’t show up: ensure CLion loads ~/.lldbinit and Python renderers are enabled; try manual import.
- If vector shows raw bytes: confirm the type name matches regex and std::array internal field (_M_elems or __elems_).
- Large adapter size: re-import updated script; it properly dereferences the size reference.
- Editor warning about missing lldb module is harmless; LLDB provides it at runtime.

Notes:
- Scripts register via __lldb_init_module; no deprecated flags are used.
- Previews are short (up to 4 elements) for performance; expand children to see more.
