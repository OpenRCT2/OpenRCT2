/* Force-included (-include) before every translation unit.
 * bebbo's m68k GCC defines int32_t as `long int`; OpenRCT2 is written against
 * targets where int32_t is `int` (std::min/max/clamp deduction, overloads).
 * Redefining the builtin type macros before <stdint.h> makes int32_t == int. */
#undef __INT32_TYPE__
#define __INT32_TYPE__ int
#undef __UINT32_TYPE__
#define __UINT32_TYPE__ unsigned int
#undef __INT_LEAST32_TYPE__
#define __INT_LEAST32_TYPE__ int
#undef __UINT_LEAST32_TYPE__
#define __UINT_LEAST32_TYPE__ unsigned int
#undef __INT_FAST32_TYPE__
#define __INT_FAST32_TYPE__ int
#undef __UINT_FAST32_TYPE__
#define __UINT_FAST32_TYPE__ unsigned int
#undef __INT32_C
#define __INT32_C(c) c
#undef __UINT32_C
#define __UINT32_C(c) c##U
#undef __INT32_MAX__
#define __INT32_MAX__ 0x7fffffff
#undef __UINT32_MAX__
#define __UINT32_MAX__ 0xffffffffU
#undef __INT_LEAST32_MAX__
#define __INT_LEAST32_MAX__ 0x7fffffff
#undef __UINT_LEAST32_MAX__
#define __UINT_LEAST32_MAX__ 0xffffffffU
#undef __INT_FAST32_MAX__
#define __INT_FAST32_MAX__ 0x7fffffff
#undef __UINT_FAST32_MAX__
#define __UINT_FAST32_MAX__ 0xffffffffU
#undef __INT32_WIDTH__
#define __INT32_WIDTH__ 32

/* ---- libstdc++ <cmath>: the configure test for C99 math failed against this
 * libc, so the std:: versions of round/lround/llround/trunc/hypot are missing
 * although math.h declares them and libnix's libm implements them. Turning on
 * _GLIBCXX_USE_C99_MATH_FUNCS wholesale needs long-double variants math.h
 * lacks, so import just the ones real code uses. ---- */
#ifdef __cplusplus
    #include <cmath>
namespace std
{
    using ::round;
    using ::roundf;
    using ::lround;
    using ::llround;
    using ::trunc;
    using ::truncf;
    using ::hypot;
    using ::hypotf;
    inline float round(float x) { return ::roundf(x); }
    inline float trunc(float x) { return ::truncf(x); }
} // namespace std
#endif

/* ---- functions libnix implements but its headers do not declare, or that
 * amiga_compat.cpp supplies. ---- */
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
struct dirent;
size_t strnlen(const char* s, size_t maxlen);
int scandir(const char* dir, struct dirent*** namelist, int (*filter)(const struct dirent*),
            int (*compar)(const struct dirent**, const struct dirent**));
int alphasort(const struct dirent** a, const struct dirent** b);
#ifdef __cplusplus
}
#endif
