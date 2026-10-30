#pragma once

#include "defines.h"

//to disable assertions, comment out below line
#define KASSERTIONS_ENABLED

#ifdef KASSERTIONS_ENABLED
#if _MSC_VER
#include <intrin.h>
#define debugBreak() __debugbreak()
#else
#define debugBreak() __builtin_trap()
#endif

KAPI void report_assertion_failure(const char *expr, const char *msg, const char *file, i32 line);

//writing assertions wrapped in do-whiles ensures that the code
// obeys C stds by putting a ; at the end of assertion lines,
// also avoids extra semicolon problems (in if-else's, for instance)

#define KASSERT(expr)                                                   \
    do {                                                                \
        if (expr) {                                                     \
        } else {                                                        \
            report_assertion_failure(#expr, "", __FILE__, __LINE__);    \
            debugBreak();                                               \
        }                                                               \
    } while (0)                                       

#define KASSERT_MSG(expr, msg)                                          \
    do {                                                                \
        if (expr) {                                                     \
        } else {                                                        \
            report_assertion_failure(#expr, msg, __FILE__, __LINE__);   \
            debugBreak();                                               \
        }                                                               \
    } while (0)                                    

#ifdef _DEBUG
#define KASSERT_DEBUG(expr)                                             \
    do {                                                                \
        if (expr) {                                                     \
        } else {                                                        \
            report_assertion_failure(#expr, "", __FILE__, __LINE__);    \
            debugBreak();                                               \
        }                                                               \
    } while (0)
#else
#define KASSERT_DEBUG(expr)
#endif
#else
#define KASSERT(expr)
#define KASSERT_MSG(expr, msg)
#define KASSERT_DEBUG(expr)
#endif
