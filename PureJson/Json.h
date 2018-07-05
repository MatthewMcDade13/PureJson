#pragma once
#include "Define.h"

struct Json;

EXTERN_C PUREJSON_LIB_API Json* pureJSON_create();
EXTERN_C PUREJSON_LIB_API void pureJSON_delete(Json* json);

EXTERN_C PUREJSON_LIB_API Json* pureJSON_parse(char* raw);
