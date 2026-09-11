#ifdef __amigaos__
// Symbols libstdc++ 16 expects from a glibc-shaped libc that libnix lacks.
#include <cerrno>
#include <cstring>
#include <sys/statvfs.h>

extern "C" int __xpg_strerror_r(int errnum, char* buf, size_t buflen)
{
    const char* s = std::strerror(errnum);
    if (!s) { errno = EINVAL; return EINVAL; }
    std::strncpy(buf, s, buflen);
    if (buflen) buf[buflen - 1] = '\0';
    return 0;
}

// libstdc++'s fs_ops.o references this with C++ linkage because the
// sys-include header carries no extern "C". Stub until a dos.library Info()
// based version is written; std::filesystem::space() reports ENOSYS.
int statvfs(const char*, struct statvfs*)
{
    errno = ENOSYS;
    return -1;
}

extern "C" size_t strnlen(const char* s, size_t maxlen)
{
    size_t n = 0;
    while (n < maxlen && s[n] != '\0')
        n++;
    return n;
}

#endif
