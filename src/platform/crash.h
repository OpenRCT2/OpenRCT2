#ifndef _OPENRCT2_CRASH_
#define _OPENRCT2_CRASH_

typedef void* CExceptionHandler;

#ifdef __cplusplus
extern "C"
{
#endif
CExceptionHandler newCExceptionHandlerSimple(void);
#ifdef __cplusplus
}
#endif

#endif  /* _OPENRCT2_CRASH_ */
