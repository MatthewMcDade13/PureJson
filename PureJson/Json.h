#pragma once
#include "Define.h"

#if defined(__cplusplus)
typedef bool pj_boolean;
#else
typedef int pj_boolean;
#endif

struct pj_Object;
struct pj_Array;

#if defined(__cplusplus)
namespace pj
{
	struct PUREJSON_LIB_API ObjectRoot
	{
		pj_Object* handle;

		ObjectRoot(pj_Object* handle);

		ObjectRoot(ObjectRoot& other) = delete;
		ObjectRoot(ObjectRoot&& other);

		~ObjectRoot();

		ObjectRoot& operator=(ObjectRoot& other) = delete;
		ObjectRoot& operator=(ObjectRoot&& other);

	};

	struct PUREJSON_LIB_API ArrayRoot
	{
		pj_Array* handle;

		ArrayRoot(pj_Array* handle);

		ArrayRoot(ArrayRoot& other) = delete;
		ArrayRoot(ArrayRoot&& other);

		~ArrayRoot();

		ArrayRoot& operator=(ArrayRoot& other) = delete;
		ArrayRoot& operator=(ArrayRoot&& other);
	};
}
#endif 

enum pj_ValueType
{
	PJ_VALUE_NUMBER,
	PJ_VALUE_STRING,
	PJ_VALUE_BOOL,
	PJ_VALUE_OBJ,
	PJ_VALUE_ARRAY,
	PJ_VALUE_NULL
};

/* Object Create/Delete */
EXTERN_C PUREJSON_LIB_API pj_Object* pj_createObj();
EXTERN_C PUREJSON_LIB_API void pj_deleteObj(pj_Object* json);

/* Array Create/Delete */
EXTERN_C PUREJSON_LIB_API pj_Array* pj_createArray();
EXTERN_C PUREJSON_LIB_API void pj_deleteArray(pj_Array* array);

/* Object/Array Parsers */
EXTERN_C PUREJSON_LIB_API pj_Object* pj_parseObj(const char* raw);
EXTERN_C PUREJSON_LIB_API pj_Array* pj_parseArray(const char* raw);

/* Object Get */
EXTERN_C PUREJSON_LIB_API double pj_objGetNum(pj_Object* json, const char* propName);
EXTERN_C PUREJSON_LIB_API pj_boolean pj_objGetBool(pj_Object* json, const char* propName);
EXTERN_C PUREJSON_LIB_API const char* pj_objGetString(pj_Object* json, const char* propName);
EXTERN_C PUREJSON_LIB_API pj_Array* pj_objGetArray(pj_Object* json, const char* propName);
EXTERN_C PUREJSON_LIB_API pj_Object* pj_objGetObj(pj_Object* json, const char* propName);

/* Array Get */
EXTERN_C PUREJSON_LIB_API double pj_arrayGetNum(pj_Array* array, size_t index);
EXTERN_C PUREJSON_LIB_API pj_boolean pj_arrayGetBool(pj_Array* array, size_t index);
EXTERN_C PUREJSON_LIB_API const char* pj_arrayGetString(pj_Array* array, size_t index);
EXTERN_C PUREJSON_LIB_API pj_Array* pj_arrayGetArray(pj_Array* array, size_t index);
EXTERN_C PUREJSON_LIB_API pj_Object* pj_arrayGetObj(pj_Array* array, size_t index);

/* Array Add */
EXTERN_C PUREJSON_LIB_API void pj_arrayAddNum(pj_Array* array, double num);
EXTERN_C PUREJSON_LIB_API void pj_arrayAddBool(pj_Array* array, pj_boolean boolean);
EXTERN_C PUREJSON_LIB_API void pj_arrayAddString(pj_Array* array, const char* str);
EXTERN_C PUREJSON_LIB_API void pj_arrayAddArray(pj_Array* array, pj_Array* other);
EXTERN_C PUREJSON_LIB_API void pj_arrayAddObj(pj_Array* array, pj_Object* obj);
EXTERN_C PUREJSON_LIB_API void pj_arrayAddNull(pj_Array* array);

/* Object Set */
EXTERN_C PUREJSON_LIB_API void pj_objSetNum(pj_Object* obj, const char* propName, double num);
EXTERN_C PUREJSON_LIB_API void pj_objSetBool(pj_Object* obj, const char* propName, pj_boolean boolean);
EXTERN_C PUREJSON_LIB_API void pj_objSetString(pj_Object* obj, const char* propName, const char* str);
EXTERN_C PUREJSON_LIB_API void pj_objSetArray(pj_Object* obj, const char* propName, pj_Array* array);
EXTERN_C PUREJSON_LIB_API void pj_objSetObj(pj_Object* obj, const char* propName, pj_Object* other);
EXTERN_C PUREJSON_LIB_API void pj_objSetNull(pj_Object* obj, const char* propName);

/* Value Inspection */
EXTERN_C PUREJSON_LIB_API pj_ValueType pj_getObjPropType(pj_Object* obj, const char* propName);
EXTERN_C PUREJSON_LIB_API pj_ValueType pj_getArrayElemType(pj_Array* array, size_t index);
EXTERN_C PUREJSON_LIB_API pj_boolean pj_isArrayElemOfType(pj_Array* array, size_t index, pj_ValueType type);
EXTERN_C PUREJSON_LIB_API pj_boolean pj_isObjPropOfType(pj_Object* obj, const char* propName, pj_ValueType type);

/* Iteration */
EXTERN_C PUREJSON_LIB_API void pj_objForEachKey(pj_Object* obj, void(*callback)(pj_Object*, const char*));

EXTERN_C PUREJSON_LIB_API size_t pj_getArraySize(pj_Array* array);
