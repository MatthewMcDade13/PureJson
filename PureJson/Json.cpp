#include "stdafx.h"
#include "Json.h"
#include "Util.h"
#include "Tokenizer.h"
#include <unordered_map>
#include <cctype>
#include <assert.h>
#include <iostream>
#include <string>

// TODO: Verify no memory leaks!!
// TODO: Implement Error Handling (preferably don't want to crash if json is invalid or
// user attempts to get value from property that does not exist.

static void parseJSONObject(Cursor& cursor, pj_Object* json);
static void parseJSONArray(Cursor& cursor, pj_Array* array);
static bool parseJSONValue(Cursor& cursor, Token& valueToken, struct JsonVal& val);
static void addArrayValue(pj_Array& array, JsonVal&& val);
static char* cpyStringDynamic(const char* str);

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

	JsonVal& operator=(JsonVal&& other)
	{
		type = other.type;
		if (this != &other)
		{
			free();
			move(std::forward<JsonVal>(other));
		}

		return *this;
	}

	JsonVal(JsonVal&& other) :
		type(other.type)
	{
		move(std::forward<JsonVal>(other));
	}

	~JsonVal()
	{
		free();
	}

private:

	void free()
	{
		switch (type)
		{
			case PJ_VALUE_STRING: 
				delete[] string;
				string = nullptr;     
			break;
			case PJ_VALUE_ARRAY: 
				pj_deleteArray(array); 
				array = nullptr;
			break;
			case PJ_VALUE_OBJ:    
				pj_deleteObj(obj); 
				obj = nullptr;
			break;
		}
	}

	void move(JsonVal&& other)
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

	if (getToken(c).type == Token::OPEN_BRACE)
	{	
		parseJSONObject(c, json);
		return json;
	}
	else
	{
		pj_deleteObj(json);
		return nullptr;
	}
}

EXTERN_C pj_Array * pj_parseArray(const char * raw)
{
	pj_Array* array = pj_createArray();
	Cursor c = { raw };

	if (getToken(c).type == Token::SQUARE_BRACKET_OPEN)
	{
		parseJSONArray(c, array);
		return array;
	}
	else
	{
		pj_deleteArray(array);
		return nullptr;
	}

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
	if (array)
	{
		delete[] array->items;
		delete array;
	}
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

EXTERN_C void pj_arrayAddNum(pj_Array * array, double num)
{
	assert(array != nullptr);

	JsonVal val;
	val.type = PJ_VALUE_NUMBER;
	val.num = num;

	addArrayValue(*array, std::move(val));
}

EXTERN_C void pj_arrayAddBool(pj_Array * array, pj_boolean boolean)
{
	assert(array != nullptr);
	return void();
}

EXTERN_C void pj_arrayAddString(pj_Array * array, const char * str)
{
	assert(array != nullptr);

	JsonVal val;
	val.type = PJ_VALUE_STRING;
	val.string = cpyStringDynamic(str);

	addArrayValue(*array, std::move(val));
}

EXTERN_C void pj_arrayAddArray(pj_Array * array, pj_Array * other)
{
	assert(array != nullptr);

	JsonVal val;
	val.type = PJ_VALUE_ARRAY;
	val.array = other;

	addArrayValue(*array, std::move(val));
}

EXTERN_C void pj_arrayAddObj(pj_Array * array, pj_Object * obj)
{
	assert(array != nullptr);

	JsonVal val;
	val.type = PJ_VALUE_OBJ;
	val.obj = obj;

	addArrayValue(*array, std::move(val));
}

EXTERN_C void pj_objAddNum(pj_Object * obj, const char * propName, double num)
{
	JsonProp prop;
	prop.name = propName;
	prop.val.type = PJ_VALUE_NUMBER;
	prop.val.num = num;

	obj->data.emplace(propName, std::move(prop));
}

EXTERN_C void pj_objAddBool(pj_Object * obj, const char * propName, pj_boolean boolean)
{
	JsonProp prop;
	prop.name = propName;
	prop.val.type = PJ_VALUE_BOOL;
	prop.val.boolean = boolean;

	obj->data.emplace(propName, std::move(prop));
}

EXTERN_C void pj_objAddString(pj_Object * obj, const char * propName, const char * str)
{
	JsonProp prop;
	prop.name = propName;
	prop.val.type = PJ_VALUE_NUMBER;

	prop.val.string = cpyStringDynamic(str);

	obj->data.emplace(propName, std::move(prop));
}

EXTERN_C void pj_objAddArray(pj_Object * obj, const char * propName, pj_Array * array)
{
	JsonProp prop;
	prop.name = propName;
	prop.val.type = PJ_VALUE_ARRAY;
	prop.val.array = array;

	obj->data.emplace(propName, std::move(prop));
}

EXTERN_C void pj_objAddObj(pj_Object * obj, const char * propName, pj_Object * other)
{
	JsonProp prop;
	prop.name = propName;
	prop.val.type = PJ_VALUE_OBJ;
	prop.val.obj = other;

	obj->data.emplace(propName, std::move(prop));
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

				if (!parseJSONValue(cursor, val, jprop.val))
				{
					// TODO: ERROR HANDLING
					return;
				}

				json->data.emplace(propName, std::move(jprop));
			
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
	bool done = false;

	while (!done && *cursor.at != NULL)
	{
		Token item = getToken(cursor);
		JsonVal val = { };

		if (!parseJSONValue(cursor, item, val))
		{
			// TODO: ERROR HANDLING
			return;
		}

		addArrayValue(*array, std::move(val));

		Token next = getToken(cursor);
		if (next.type == Token::SQUARE_BRACKET_CLOSE)
		{
			done = true;
		}
		else if (next.type != Token::COMMA)
		{
			std::cerr << "Missing comma after array element" << std::endl;
			break;
			// TODO: ERROR
		}
	}
}

bool parseJSONValue(Cursor & cursor, Token & valueToken, JsonVal & val)
{
	switch (valueToken.type)
	{
		case Token::BOOL:
			val.type = PJ_VALUE_BOOL;

			if (cmpSubStr(valueToken.str, "false", valueToken.length))
			{
				val.boolean = false;
			}
			else if (cmpSubStr(valueToken.str, "true", valueToken.length))
			{
				val.boolean = true;
			}

		break;
		case Token::NUMBER:
			val.type = PJ_VALUE_NUMBER;
			char buffer[255];
			memcpy(buffer, valueToken.str, valueToken.length);
			val.num = atof(buffer);
		break;
		case Token::STRING: 
			val.type = PJ_VALUE_STRING;
			val.string = parseCString(valueToken.str);
		break;
		case Token::SQUARE_BRACKET_OPEN:
			val.type = PJ_VALUE_ARRAY;
			val.array = pj_createArray();
			parseJSONArray(cursor, val.array);
		break;
		case Token::OPEN_BRACE:
			val.type = PJ_VALUE_OBJ;
			val.obj = pj_createObj();
			parseJSONObject(cursor, val.obj);
			break;
		default:
			std::cerr << "Value Token of unknown or unspecified type" << std::endl;
			return false;
	}

	return true;
}

void addArrayValue(pj_Array & array, JsonVal && val)
{
	const size_t newSize = array.size + 1;
	if (newSize + 1 > array.capacity)
	{
		JsonVal* newItems = new JsonVal[newSize]();

		for (size_t i = 0; i < array.size; i++)
		{
			newItems[i] = std::move(array.items[i]);
		}

		delete[] array.items;
		array.items = newItems;
	}

	array.items[newSize - 1] = std::move(val);
	array.size = newSize;
	array.capacity = newSize;
}

char * cpyStringDynamic(const char * str)
{
	const uint32_t stringLen = strlen(str);
	char* result = new char[stringLen + 1]();
	strcpy(result, str);

	return result;
}

JsonProp* findProp(pj_Object& obj, const char* propName)
{
	auto itr = obj.data.find(propName);
	if (itr == obj.data.end()) return nullptr;

	return &itr->second;
}

