#pragma once
#include "Define.h"

#if defined(__cplusplus)
typedef bool pj_boolean;
#else
typedef int pj_boolean;
#endif

struct pj_Object;
struct pj_Array;

enum pj_ValueType
{
	PJ_VALUE_NUMBER,
	PJ_VALUE_STRING,
	PJ_VALUE_BOOL,
	PJ_VALUE_OBJ,
	PJ_VALUE_ARRAY
};

EXTERN_C PUREJSON_LIB_API pj_Object* pj_createObj();
EXTERN_C PUREJSON_LIB_API void pj_deleteObj(pj_Object* json);

EXTERN_C PUREJSON_LIB_API pj_Array* pj_createArray();
EXTERN_C PUREJSON_LIB_API void pj_deleteArray(pj_Array* array);

EXTERN_C PUREJSON_LIB_API pj_Object* pj_parseObj(const char* raw);
EXTERN_C PUREJSON_LIB_API pj_Array* pj_parseArray(const char* raw);

EXTERN_C PUREJSON_LIB_API double pj_objGetNum(pj_Object* json, const char* propName);
EXTERN_C PUREJSON_LIB_API pj_boolean pj_objGetBool(pj_Object* json, const char* propName);
EXTERN_C PUREJSON_LIB_API const char* pj_objGetString(pj_Object* json, const char* propName);
EXTERN_C PUREJSON_LIB_API pj_Array* pj_objGetArray(pj_Object* json, const char* propName);
EXTERN_C PUREJSON_LIB_API pj_Object* pj_objGetObj(pj_Object* json, const char* propName);

EXTERN_C PUREJSON_LIB_API double pj_arrayGetNum(pj_Array* array, size_t index);
EXTERN_C PUREJSON_LIB_API pj_boolean pj_arrayGetBool(pj_Array* array, size_t index);
EXTERN_C PUREJSON_LIB_API const char* pj_arrayGetString(pj_Array* array, size_t index);
EXTERN_C PUREJSON_LIB_API pj_Array* pj_arrayGetArray(pj_Array* array, size_t index);
EXTERN_C PUREJSON_LIB_API pj_Object* pj_arrayGetObj(pj_Array* array, size_t index);

EXTERN_C PUREJSON_LIB_API pj_ValueType pj_getPropType(pj_Object* obj, const char* propName);
EXTERN_C PUREJSON_LIB_API pj_ValueType pj_getArrayElemType(pj_Array* array, size_t index);
