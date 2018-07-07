#include "stdafx.h"
#include "Json.h"
#include "Util.h"
#include "Tokenizer.h"
#include <unordered_map>
#include <cctype>
#include <assert.h>
#include <iostream>
#include <string>

// TODO: Implement parsing of arrays
// TODO: Implement Error Handling (preferably don't want to crash if json is invalid or
// user attempts to get value from property that does not exist.

static void parseJSONObject(Cursor& cursor, pj_Object* json);
static void parseJSONArray(Cursor& cursor, pj_Array* array);

static struct JsonProp* findProp(pj_Object& obj, const char* propName);

struct JsonVal
{
	pj_ValueType type;

	union
	{
		double num;
		bool boolean;
		char* string;
		pj_Array* array;
		pj_Object* obj;
	};

	JsonVal() = default;
	JsonVal(JsonVal& other) = delete;
	JsonVal operator=(JsonVal& other) = delete;

	JsonVal(JsonVal&& other) :
		type(other.type)
	{
		switch (other.type)
		{
			case PJ_VALUE_NUMBER: num = other.num; break;
			case PJ_VALUE_STRING:
				string = other.string;
				other.string = nullptr;
			break;
			case PJ_VALUE_BOOL: boolean = other.boolean; break;
			case PJ_VALUE_OBJ:
				obj = other.obj;
				other.obj = nullptr;
			break;
			case PJ_VALUE_ARRAY:
				array = other.array;
				other.array = nullptr;
			break;
		}
	}

	~JsonVal()
	{
		switch (type)
		{
			case PJ_VALUE_STRING: delete[] string;       break;
			case PJ_VALUE_ARRAY:  pj_deleteArray(array); break;
			case PJ_VALUE_OBJ:    pj_deleteObj(obj);  break;
		}
	}
};

struct JsonProp
{
	std::string name;

	JsonVal val;
};

struct pj_Array
{
	JsonVal* items;
	size_t size;
	size_t capacity;
};

struct pj_Object
{
	std::unordered_map<std::string, JsonProp> data;
};

EXTERN_C pj_Object * pj_parseObj(const char * raw)
{
	pj_Object* json = pj_createObj();
	Cursor c = { raw };

	parseJSONObject(c, json);

	return json;
}

EXTERN_C pj_Array * pj_parseArray(const char * raw)
{
	return nullptr;
}

EXTERN_C pj_Object * pj_createObj()
{
	return new pj_Object();
}

EXTERN_C void pj_deleteObj(pj_Object* json)
{
	delete json;
}

EXTERN_C pj_Array * pj_createArray()
{
	pj_Array* array = new pj_Array();

	array->capacity = 0;
	array->size = 0;

	return new pj_Array();
}

EXTERN_C void pj_deleteArray(pj_Array * array)
{
	delete[] array->items;
	delete array;
}

EXTERN_C double pj_objGetNum(pj_Object * json, const char * propName)
{
	JsonProp* prop = findProp(*json, propName);
	assert(prop->val.type == PJ_VALUE_NUMBER);

	return prop->val.num;
}

EXTERN_C pj_boolean pj_objGetBool(pj_Object * json, const char * propName)
{
	JsonProp* prop = findProp(*json, propName);
	assert(prop->val.type == PJ_VALUE_BOOL);

	return prop->val.boolean;
}

EXTERN_C const char* pj_objGetString(pj_Object * json, const char * propName)
{
	JsonProp* prop = findProp(*json, propName);
	assert(prop->val.type == PJ_VALUE_STRING);

	return prop->val.string;
}

EXTERN_C pj_Array* pj_objGetArray(pj_Object * json, const char * propName)
{
	JsonProp* prop = findProp(*json, propName);
	assert(prop->val.type == PJ_VALUE_ARRAY);

	return prop->val.array;
}

EXTERN_C pj_Object* pj_objGetObj(pj_Object * json, const char * propName)
{
	JsonProp* prop = findProp(*json, propName);
	assert(prop->val.type == PJ_VALUE_OBJ);

	return prop->val.obj;
}

EXTERN_C double pj_arrayGetNum(pj_Array * array, size_t index)
{
	assert(index >= 0 && index <= array->size);
	JsonVal& val = array->items[index];

	assert(val.type == PJ_VALUE_NUMBER);
	return val.num;
}

EXTERN_C pj_boolean pj_arrayGetBool(pj_Array * array, size_t index)
{
	assert(index >= 0 && index <= array->size);
	JsonVal& val = array->items[index];

	assert(val.type == PJ_VALUE_BOOL);
	return val.boolean;
}

EXTERN_C const char * pj_arrayGetString(pj_Array * array, size_t index)
{
	assert(index >= 0 && index <= array->size);
	JsonVal& val = array->items[index];

	assert(val.type == PJ_VALUE_STRING);
	return val.string;
}

EXTERN_C pj_Array * pj_arrayGetArray(pj_Array * array, size_t index)
{
	assert(index >= 0 && index <= array->size);
	JsonVal& val = array->items[index];

	assert(val.type == PJ_VALUE_ARRAY);
	return val.array;
}

EXTERN_C pj_Object * pj_arrayGetObj(pj_Array * array, size_t index)
{
	assert(index >= 0 && index <= array->size);
	JsonVal& val = array->items[index];

	assert(val.type == PJ_VALUE_OBJ);
	return val.obj;
}

EXTERN_C pj_ValueType pj_getPropType(pj_Object * obj, const char * propName)
{
	JsonProp& prop = obj->data[std::string(propName)];

	return prop.val.type;
}

EXTERN_C pj_ValueType pj_getArrayElemType(pj_Array * array, size_t index)
{
	assert(index >= 0 && index <= array->size);
	JsonVal& val = array->items[index];
	return val.type;
}

void parseJSONObject(Cursor& cursor, pj_Object * json)
{
	++cursor.at;

	bool done = false;

	while (!done && *cursor.at != NULL)
	{
		Token t = getToken(cursor);

		if (t.type == Token::STRING)
		{
			Token colon = getToken(cursor);

			if (colon.type == Token::COLON)
			{
				std::string propName = parseString(t.str);
				Token val = getToken(cursor);

				JsonProp jprop = {};
				jprop.name = propName;

				switch (val.type)
				{
				case Token::BOOL:
					jprop.val.type = PJ_VALUE_BOOL;

					if (cmpSubStr(val.str, "false", val.length))
					{
						jprop.val.boolean = false;
					}
					else if (cmpSubStr(val.str, "true", val.length))
					{
						jprop.val.boolean = true;
					}

				break;
				case Token::NUMBER:
					jprop.val.type = PJ_VALUE_NUMBER;
					char buffer[255];
					memcpy(buffer, val.str, val.length);
					jprop.val.num = atof(buffer);
				break;
				case Token::STRING: {
					jprop.val.type = PJ_VALUE_STRING;
					jprop.val.string = parseCString(val.str);
				} break;
				case Token::SQUARE_BRACKET_OPEN:
					// TODO: Parse arrays
				break;
				case Token::OPEN_BRACE:
					jprop.val.type = PJ_VALUE_OBJ;
					jprop.val.obj = pj_createObj();
					parseJSONObject(cursor, jprop.val.obj);
				break;
				}

				json->data.emplace(propName, std::move(jprop));
				
				Token endToken = getToken(cursor);
			}
			else
			{
				std::cerr << "Expected Colon after property name" << std::endl;
				break;
				// TODO: ERROR
			}
		}
		else if (t.type == Token::CLOSE_BRACE)
		{
			done = true;
		}
		else if (t.type != Token::COMMA)
		{
			std::cerr << "Missing comma after property name" << std::endl;
			break;
			// TODO: ERROR
		}
	}
}

void parseJSONArray(Cursor & cursor, pj_Array * array)
{

}

JsonProp* findProp(pj_Object& obj, const char* propName)
{
	auto itr = obj.data.find(propName);
	if (itr == obj.data.end()) return nullptr;

	return &itr->second;
}

