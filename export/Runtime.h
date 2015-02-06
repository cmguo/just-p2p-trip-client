// just_runtime.h

#ifndef _PLUGINS_TRIP_EXPORT_RUNTIME_H_
#define _PLUGINS_TRIP_EXPORT_RUNTIME_H_

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#if (defined _WIN32) || (defined WIN32)
#include <windows.h>
#  define dlopen(n, f) LoadLibraryA(n)
#  define dlsym(m, s) GetProcAddress(m, s)
#  define dlclose(m) FreeLibrary(m)
#  define RTLD_LAZY 0
#else
#  include <dlfcn.h>
typedef void * HMODULE;
#endif

#ifndef __cplusplus
#  define inline
#endif

#if (defined WINRT) || (defined WIN_PHONE)
static inline HMODULE LoadLibraryA(
	_In_  LPCSTR lpFileName)
{
	WCHAR pwFileName[MAX_PATH];
	::MultiByteToWideChar(CP_ACP, 0, lpFileName, -1, pwFileName, MAX_PATH);
	return LoadPackagedLibrary(pwFileName, 0);
}
#endif

#ifdef __cplusplus
#  define TRIP_LIB_NAME just::name_string()
#else
#  define TRIP_LIB_NAME name_string()
#endif

#ifdef __cplusplus
extern HMODULE TRIP_Load(
    char const * name = NULL);
#else
extern HMODULE TRIP_Load(
    char const * name);
#endif
extern void TRIP_Unload(void); 


#ifndef TRIP_LIBRARY_NOT_EXIST
#  define TRIP_LIBRARY_NOT_EXIST(x) fprintf(stderr, "library %s not exists\r\n", x); abort();
#endif

#ifndef TRIP_FUNCTION_NOT_EXIST
#  define TRIP_FUNCTION_NOT_EXIST(x) fprintf(stderr, "function %s not exists", #x); abort();
#endif

#define TRIP_DECL

#ifndef TRIP_FUNC
#ifdef TRIP_DECLARE_ONLY
#define TRIP_FUNC(type, name, np, params) \
    type name(FUNCTION_PARAMS_TYPE_NAME(np, params));
#else  // TRIP_DECLARE_ONLY
#define TRIP_FUNC(type, name, np, params) \
    type name(FUNCTION_PARAMS_TYPE_NAME(np, params)); \
    inline type name(FUNCTION_PARAMS_TYPE_NAME(np, params)) \
    { \
        typedef type (* FT_ ## name)(FUNCTION_PARAMS_TYPE(np, params)); \
        static FT_ ## name fp = NULL; \
        if (fp == NULL) { \
            fp = (FT_ ## name)dlsym(TRIP_Load(NULL), #name); \
            if (fp == NULL) { \
                TRIP_FUNCTION_NOT_EXIST(name); \
                return type##_defalut(); \
            } \
        } \
        return fp(FUNCTION_PARAMS_NAME(np, params)); \
    }
#endif // TRIP_DECLARE_ONLY
#endif // TRIP_FUNC

#include "ITripTypes.h"

static inline void void_defalut(void) { }
static inline PP_err PP_err_defalut(void) { return just_other_error; }
static inline PP_str PP_str_defalut(void) { return ""; }
static inline PP_handle PP_handle_defalut(void) { return NULL; }
static inline PP_ushort PP_ushort_defalut(void) { return 0; }
static inline PP_uint PP_uint_defalut(void) { return 0; }
static inline PP_ulong PP_ulong_defalut(void) { return 0; }

#include "Trip.h"

#include "Name.h"

#ifndef TRIP_DECLARE_ONLY

inline void TRIP_Unload(void)
{
    if (TRIP_Load(NULL)) {
#ifndef TRIP_DISABLE_AUTO_START
        TRIP_StopEngine();
#endif
        dlclose(TRIP_Load(NULL));
    }
}

inline HMODULE TRIP_Load(
    char const * name)
{
    static HMODULE hMod = NULL;
    if (hMod == NULL) {
        if (name == NULL)
            name = TRIP_LIB_NAME;
        hMod = dlopen(name, RTLD_LAZY);
        if (hMod == NULL) {
            TRIP_LIBRARY_NOT_EXIST(name);
        } else {
#ifndef TRIP_DISABLE_AUTO_START
            PP_err ret = TRIP_StartEngine("12", "161", "08ae1acd062ea3ab65924e07717d5994");
            if (ret != just_success && ret != just_already_start) {
                TRIP_Unload();
            }
#endif
        }
    }
    return hMod;
}

#endif // TRIP_DECLARE_ONLY

#endif // _PLUGINS_TRIP_EXPORT_RUNTIME_H_
