#pragma once
#include "Define.h"

struct Json;

struct JsonArray
{
	void* items;
	size_t size;
};

EXTERN_C PUREJSON_LIB_API Json* pureJSON_create();
EXTERN_C PUREJSON_LIB_API void pureJSON_delete(Json* json);

EXTERN_C PUREJSON_LIB_API Json* pureJSON_parse(char* raw);

EXTERN_C PUREJSON_LIB_API double pureJSON_getNum(Json* json, const char* propName);
EXTERN_C PUREJSON_LIB_API bool pureJSON_getBool(Json* json, const char* propName);
EXTERN_C PUREJSON_LIB_API const char* pureJSON_getString(Json* json, const char* propName);
EXTERN_C PUREJSON_LIB_API JsonArray pureJSON_getArray(Json* json, const char* propName);
EXTERN_C PUREJSON_LIB_API Json* pureJSON_getObj(Json* json, const char* propName);

