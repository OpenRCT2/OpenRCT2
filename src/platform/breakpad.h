#ifndef _OPENRCT2_BREAKPAD_
#define _OPENRCT2_BREAKPAD_

#ifdef USE_BREAKPAD
typedef void* CExceptionHandler;

#ifdef __cplusplus
extern "C"
{
#endif
CExceptionHandler newCExceptionHandlerSimple(void);
#ifdef __cplusplus
}
#endif
#endif // USE_BREAKPAD

#endif  /* _OPENRCT2_BREAKPAD_ */
