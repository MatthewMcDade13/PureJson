#pragma once

#if defined(_WIN32) || defined(_WIN64)
#ifdef PUREJSON_LIB_EXPORTS  
#define PUREJSON_LIB_API __declspec(dllexport)   
#else  
#define PUREJSON_LIB_API __declspec(dllimport)   
#endif
#else
#define PUREJSON_LIB_API
#endif

#if defined(__cplusplus)
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

