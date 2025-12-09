import lldb


def _find_member_recursive(valobj, name):
    """Find a direct member by name. Avoids LLDB methods that may not exist.

    Some LLDB Python builds do not expose SBValue.IsBaseClass(). Our types do not
    use inheritance for relevant fields, so a direct lookup is sufficient.
    """
    m = valobj.GetChildMemberWithName(name)
    if m and m.IsValid():
        return m
    return None


def _read_unsigned_value(sbval, default=0):
    """Best-effort read of an integral value (handles references as well).

    Strategy:
    - Prefer parsing SBValue.GetValue() string (works well for references shown as numbers).
    - If that fails, try Dereference() (covers reference types on some LLDB builds).
    - Fallback to GetValueAsUnsigned().
    """
    # 0) Quick exits
    try:
        if not sbval or not sbval.IsValid():
            return int(default)
    except Exception:
        return int(default)

    # 1) Try to dereference first (common for references): this usually yields the numeric value
    try:
        d = sbval.Dereference()
        if d and d.IsValid():
            return int(d.GetValueAsUnsigned(default))
    except Exception:
        pass

    # 2) Try parsing string value of the original sbval
    try:
        s = sbval.GetValue()
        if s is not None:
            s = s.strip()
            if len(s) > 0:
                if s.startswith("0x") or s.startswith("-0x"):
                    # Likely an address; try a second deref (some builds need two hops)
                    try:
                        d2 = sbval.Dereference()
                        if d2 and d2.IsValid():
                            return int(d2.GetValueAsUnsigned(default))
                    except Exception:
                        pass
                    # Fall back to hex parse (address), not ideal but better than crash
                    return int(s, 16)
                return int(s)
    except Exception:
        pass

    # 3) Fallback to unsigned accessor (may return pointer for references)
    try:
        return int(sbval.GetValueAsUnsigned(default))
    except Exception:
        return int(default)


def _get_template_elem_type(valobj):
    ty = valobj.GetType()
    try:
        return ty.GetTemplateArgumentType(0)
    except Exception:
        return None


def _get_arr_first_byte_addr(arr_val):
    # libstdc++: std::array has a field named _M_elems
    m_elems = arr_val.GetChildMemberWithName("_M_elems")
    if m_elems and m_elems.IsValid() and m_elems.GetNumChildren() > 0:
        first = m_elems.GetChildAtIndex(0)
        return first.GetLoadAddress()
    # libc++: __elems_ or __elems_ within __array_t
    elems = arr_val.GetChildMemberWithName("__elems_")
    if elems and elems.IsValid() and elems.GetNumChildren() > 0:
        first = elems.GetChildAtIndex(0)
        return first.GetLoadAddress()
    # Fallback: take the address of the array itself
    return arr_val.AddressOf().GetLoadAddress()


def _get_arr_num_bytes(arr_val):
    # Try to infer number of bytes from std::array child count
    m_elems = arr_val.GetChildMemberWithName("_M_elems")
    if m_elems and m_elems.IsValid():
        return m_elems.GetNumChildren()  # number of std::byte entries
    elems = arr_val.GetChildMemberWithName("__elems_")
    if elems and elems.IsValid():
        return elems.GetNumChildren()
    # Last resort: try to get size from its type (std::array<std::byte, N>)
    try:
        t = arr_val.GetType()
        # index 1 is the non-type template arg N
        n = t.GetTemplateArgumentAsUnsigned(1)
        return int(n)
    except Exception:
        return 0


class StaticVectorSynthProvider:
    def __init__(self, valobj, _internal_dict):
        self.valobj = valobj
        self._elem_type = _get_template_elem_type(valobj)
        self._elem_size = self._elem_type.GetByteSize() if self._elem_type else 0
        self._data_addr = None
        self._size = None

    def update(self):
        self._elem_type = _get_template_elem_type(self.valobj)
        self._elem_size = self._elem_type.GetByteSize() if self._elem_type else 0
        self._data_addr = None
        self._size = None

    def _compute_layout(self):
        if self._data_addr is not None and self._size is not None:
            return
        # size
        szm = _find_member_recursive(self.valobj, "elementsCount")
        self._size = szm.GetValueAsUnsigned(0) if (szm and szm.IsValid()) else 0
        # data start from the backing byte array
        arr = _find_member_recursive(self.valobj, "arr")
        if arr and arr.IsValid():
            self._data_addr = _get_arr_first_byte_addr(arr)
        else:
            self._data_addr = 0

    def num_children(self):
        self._compute_layout()
        return int(self._size)

    def get_child_at_index(self, index):
        self._compute_layout()
        if index < 0 or index >= self._size or not self._elem_type or self._data_addr == 0:
            return lldb.SBValue()
        addr = self._data_addr + index * self._elem_size
        return self.valobj.CreateValueFromAddress(f"[{index}]", addr, self._elem_type)

    def has_children(self):
        self._compute_layout()
        return self._size > 0


def StaticVectorSummary(valobj, _internal_dict):
    try:
        szm = _find_member_recursive(valobj, "elementsCount")
        size = szm.GetValueAsUnsigned(0) if (szm and szm.IsValid()) else 0

        # capacity from arr bytes / sizeof(T)
        arr = _find_member_recursive(valobj, "arr")
        if not (arr and arr.IsValid()):
            return f"static_vector(size={size})"

        elem_type = _get_template_elem_type(valobj)
        elem_sz = elem_type.GetByteSize() if elem_type else 0
        total_bytes = _get_arr_num_bytes(arr)
        cap = int(total_bytes // elem_sz) if elem_sz else 0

        # Tiny preview: attempt to print first few elements using GetValue()/GetSummary()
        preview = ""
        if size > 0 and elem_type and elem_type.IsValid():
            base = _get_arr_first_byte_addr(arr)
            count = min(4, int(size))
            parts = []
            for i in range(count):
                addr = base + i * elem_sz
                sbval = valobj.CreateValueFromAddress("tmp", addr, elem_type)
                disp = sbval.GetValue()
                if disp is None:
                    disp = sbval.GetSummary()
                if disp is None:
                    # Fall back to type name if nothing else
                    disp = elem_type.GetName() or "?"
                parts.append(disp)
            if int(size) > count:
                preview = " [" + ", ".join(parts) + ", …]"
            else:
                preview = " [" + ", ".join(parts) + "]"

        return f"static_vector(size={int(size)}, cap={int(cap)})" + preview
    except Exception as e:
        return f"<static_vector: error {e}>"


class StaticVectorAdapterSynthProvider:
    def __init__(self, valobj, _internal_dict):
        self.valobj = valobj
        self._elem_type = _get_template_elem_type(valobj)
        self._elem_size = self._elem_type.GetByteSize() if self._elem_type else 0
        self._base_addr = None
        self._size = None

    def update(self):
        self._elem_type = _get_template_elem_type(self.valobj)
        self._elem_size = self._elem_type.GetByteSize() if self._elem_type else 0
        self._base_addr = None
        self._size = None

    def _compute_layout(self):
        if self._base_addr is not None and self._size is not None:
            return
        # size from elements_count_
        szm = _find_member_recursive(self.valobj, "elements_count_")
        self._size = _read_unsigned_value(szm, 0)
        # data pointer from elements_
        ptr = _find_member_recursive(self.valobj, "elements_")
        self._base_addr = ptr.GetValueAsUnsigned(0) if (ptr and ptr.IsValid()) else 0

    def num_children(self):
        self._compute_layout()
        return int(self._size)

    def get_child_at_index(self, index):
        self._compute_layout()
        if (
            index < 0
            or index >= self._size
            or not self._elem_type
            or self._base_addr == 0
            or self._elem_size == 0
        ):
            return lldb.SBValue()
        addr = self._base_addr + index * self._elem_size
        return self.valobj.CreateValueFromAddress(f"[{index}]", addr, self._elem_type)

    def has_children(self):
        self._compute_layout()
        return self._size > 0


def StaticVectorAdapterSummary(valobj, _internal_dict):
    try:
        # size
        szm = _find_member_recursive(valobj, "elements_count_")
        size = _read_unsigned_value(szm, 0)

        # capacity
        capm = _find_member_recursive(valobj, "max_elements_count_")
        cap = _read_unsigned_value(capm, 0)

        # preview via pointer elements_
        elem_type = _get_template_elem_type(valobj)
        elem_sz = elem_type.GetByteSize() if elem_type else 0
        ptr = _find_member_recursive(valobj, "elements_")
        base = ptr.GetValueAsUnsigned(0) if (ptr and ptr.IsValid()) else 0

        preview = ""
        if size > 0 and elem_type and elem_type.IsValid() and base != 0 and elem_sz > 0:
            count = min(4, int(size))
            parts = []
            for i in range(count):
                addr = base + i * elem_sz
                sbval = valobj.CreateValueFromAddress("tmp", addr, elem_type)
                disp = sbval.GetValue()
                if disp is None:
                    disp = sbval.GetSummary()
                if disp is None:
                    disp = elem_type.GetName() or "?"
                parts.append(disp)
            if int(size) > count:
                preview = " [" + ", ".join(parts) + ", …]"
            else:
                preview = " [" + ", ".join(parts) + "]"

        return f"static_vector_adapter(size={int(size)}, cap={int(cap)})" + preview
    except Exception as e:
        return f"<static_vector_adapter: error {e}>"


def __lldb_init_module(debugger, _internal_dict):
    mod = __name__
    # Match both classes with templates
    debugger.HandleCommand(
        f'type summary add -e -x "^wbr::static_vector<.*>$" --python-function {mod}.StaticVectorSummary'
    )
    debugger.HandleCommand(
        f'type synthetic add -e -x "^wbr::static_vector<.*>$" --python-class {mod}.StaticVectorSynthProvider'
    )
    # Also expose adapter as a vector (optional minimal support)
    debugger.HandleCommand(
        f'type summary add -e -x "^wbr::static_vector_adapter<.*>$" --python-function {mod}.StaticVectorAdapterSummary'
    )
    debugger.HandleCommand(
        f'type synthetic add -e -x "^wbr::static_vector_adapter<.*>$" --python-class {mod}.StaticVectorAdapterSynthProvider'
    )
