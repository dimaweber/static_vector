# pretty_static_string.py
# Works with lldb’s scripted summary providers.

import lldb


def _find_member_recursive(valobj, name):
    """Find a member by name, searching bases if needed (to handle inheritance)."""
    m = valobj.GetChildMemberWithName(name)
    if m and m.IsValid():
        return m
    # Search base classes
    for i in range(valobj.GetNumChildren()):
        child = valobj.GetChildAtIndex(i)
        if child and child.IsValid() and child.IsBaseClass():
            m = _find_member_recursive(child, name)
            if m and m.IsValid():
                return m
    return None


def static_string_summary(valobj, _internal_dict):
    try:
        target = valobj.GetTarget()
        process = target.GetProcess()

        head = _find_member_recursive(valobj, "head_")
        tail = _find_member_recursive(valobj, "tail_")
        maxl = _find_member_recursive(valobj, "max_length_")

        if not (head and tail and maxl):
            return "<static_string: layout not found>"

        head_addr = head.GetValueAsUnsigned(0)
        tail_addr = tail.GetValueAsUnsigned(0)
        cap = maxl.GetValueAsUnsigned(0)

        if head_addr == 0 or tail_addr == 0 or tail_addr < head_addr:
            return f"\"\" len=0 cap={cap}"

        # length is bounded by capacity
        length = tail_addr - head_addr
        if cap:
            length = min(length, cap)

        length = int(max(0, min(length, 1 << 31)))  # basic sanity

        # Read at most a reasonable preview (avoid huge reads)
        preview_len = min(length, 1024)
        error = lldb.SBError()
        data = process.ReadMemory(head_addr, preview_len, error)
        if error.Fail() or data is None:
            return f"<static_string: unreadable memory> len={length} cap={cap}"

        # Convert bytes to a Python string, stop at first NUL if present
        if isinstance(data, bytes):
            b = data
        else:
            b = bytes(data)

        nul = b.find(b"\x00")
        if nul != -1:
            b = b[:nul]

        try:
            s = b.decode('utf-8', errors='replace')
        except Exception:
            s = b.decode('latin1', errors='replace')

        # Quote and escape summary a bit
        s_disp = s.replace("\\", "\\\\").replace("\n", "\\n").replace("\r", "\\r").replace("\t", "\\t")
        if len(s_disp) > 200:
            s_disp = s_disp[:200] + "…"

        return f'"{s_disp}" len={length} cap={cap}'
    except Exception as e:
        return f"<static_string: error {e}>"
