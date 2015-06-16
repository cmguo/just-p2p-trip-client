// Types.h

#ifndef _TRIP_EXPORT_TYPES_H_
#define _TRIP_EXPORT_TYPES_H_

#ifndef DEFINED_PP_BOOL
#define DEFINED_PP_BOOL
typedef bool PP_bool;
#endif // DEFINED_PP_BOOL

#ifndef DEFINED_PP_CHAR
#define DEFINED_PP_CHAR
typedef char PP_char;
#endif // DEFINED_PP_CHAR

#ifndef DEFINED_PP_BYTE
#define DEFINED_PP_BYTE
typedef signed char PP_byte;
#endif // DEFINED_PP_BYTE

#ifndef DEFINED_PP_SHORT
#define DEFINED_PP_SHORT
typedef short PP_short;
#endif // DEFINED_PP_SHORT

#ifndef DEFINED_PP_INI
#define DEFINED_PP_INI
typedef long PP_int;
#endif // DEFINED_PP_INI

#ifndef DEFINED_PP_LONG
#define DEFINED_PP_LONG
typedef long long PP_long;
#endif // DEFINED_PP_LONG

#ifndef DEFINED_PP_UBYTE
#define DEFINED_PP_UBYTE
typedef unsigned char PP_ubyte;
#endif // DEFINED_PP_UBYTE

#ifndef DEFINED_PP_USHORT
#define DEFINED_PP_USHORT
typedef unsigned short PP_ushort;
#endif // DEFINED_PP_USHORT

#ifndef DEFINED_PP_UINI
#define DEFINED_PP_UINI
typedef unsigned long PP_uint;
#endif // DEFINED_PP_UINI

#ifndef DEFINED_PP_ULONG
#define DEFINED_PP_ULONG
typedef unsigned long long PP_ulong;
#endif // DEFINED_PP_ULONG

typedef PP_int PP_err;

typedef PP_char const * PP_str;
typedef PP_ubyte const * PP_data;

typedef void const * PP_handle;
typedef void const * PP_context;

enum TRIP_ErrorEnum
{
    trip_success = 0, 
    trip_not_start, 
    trip_already_start, 
    trip_not_open, 
    trip_already_open, 
    trip_operation_canceled, 
    trip_would_block, 
    trip_stream_end, 
    trip_logic_error, 
    trip_network_error, 
    trip_other_error = 1024, 
};

#endif // _TRIP_EXPORT_TYPES_H_
