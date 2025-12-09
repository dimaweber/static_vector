#pragma once

#ifdef _MSC_VER
    // Define ssize_t for MSVC if not already defined
    #ifndef ssize_t
using ssize_t = intptr_t;
    #endif
#else
    // Use standard definition for non-MSVC compilers
    #include <sys/types.h>
#endif
