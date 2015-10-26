// Generic.h

#ifndef _TRIP_EXPORT_GENERIC_H_
#define _TRIP_EXPORT_GENERIC_H_

#include "Types.h"

TRIP_CALLBACK_2(void, TRIP_Callback, (PP_context, PP_err));

TRIP_FUNC_0(PP_err, TRIP_StartEngine, ());

TRIP_FUNC_0(PP_err, TRIP_StopEngine, ());

//获得错误码
//返回值: 错误码
TRIP_FUNC_0(PP_err, TRIP_GetLastError, ());

TRIP_FUNC_0(PP_str, TRIP_GetLastErrorMsg, ());

TRIP_FUNC_0(PP_str, TRIP_GetVersion, ());

TRIP_FUNC_4(PP_err, TRIP_SetConfig, (
    (PP_str, module), 
    (PP_str, section), 
    (PP_str, key), 
    (PP_str, value)));

#endif // _TRIP_EXPORT_GENERIC_H_
