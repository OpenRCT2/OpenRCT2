#ifdef __amigaos__
// Symbols libstdc++ 16 expects from a glibc-shaped libc that libnix lacks.
#include <cerrno>
#include <cstring>
#include <sys/statvfs.h>
#include <sys/time.h>

// Prototypes first: the tree builds with -Wmissing-declarations.
extern "C" int __xpg_strerror_r(int errnum, char* buf, size_t buflen);

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

// libstdc++ was configured without gettimeofday/clock_gettime, so its
// steady_clock::now() falls back to time() — one-second resolution. That
// breaks every condition_variable::wait_for/wait_until predicate loop (the
// "did we time out?" check compares against a clock that has not moved, so the
// loop spins forever). Replace both clocks with gettimeofday(); defining both
// keeps chrono.o out of the link so there is no duplicate definition.
#include <chrono>
namespace std::chrono
{
    system_clock::time_point system_clock::now() noexcept
    {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        return time_point(seconds(tv.tv_sec) + microseconds(tv.tv_usec));
    }
    namespace _V2
    {
        steady_clock::time_point steady_clock::now() noexcept
        {
            struct timeval tv;
            gettimeofday(&tv, nullptr);
            return time_point(seconds(tv.tv_sec) + microseconds(tv.tv_usec));
        }
    } // namespace _V2
} // namespace std::chrono

#endif

// An uncaught exception must not leave a silent, hung process: say what it was, then exit.
#include <exception>
#include <cstdlib>
#include <cstdio>
extern "C" void amiga_trace(const char* line);
namespace
{
    [[noreturn]] void AmigaTerminate()
    {
        const char* what = "unknown";
        try
        {
            if (auto e = std::current_exception())
                std::rethrow_exception(e);
        }
        catch (const std::exception& e)
        {
            what = e.what();
        }
        catch (...)
        {
        }
        char line[512];
        std::snprintf(line, sizeof line, "FATAL: uncaught exception: %s", what);
        amiga_trace(line);
        std::fprintf(stderr, "%s\n", line);
        std::fflush(stderr);
        std::_Exit(20);
    }
    struct TerminateInstaller
    {
        TerminateInstaller()
        {
            std::set_terminate(AmigaTerminate);
        }
    } gTerminateInstaller;
} // namespace
