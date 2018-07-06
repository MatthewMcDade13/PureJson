#include "stdafx.h"
#include "Json.h"
#include <unordered_map>
#include <cctype>
#include <assert.h>
#include <iostream>
#include <string>

static char* eatWhitespace(char* str);
static struct Token getToken(char* str);
static int tryParseBoolToken(char* str);
static bool cmpSubStr(const char* a, const char* b, size_t length);
static char* fillJson(char* raw, Json* json);

static void snipDblQuotes(char* str, char*& begin, char*& end);
static std::string parseString(char* str);
static char* parseCString(char* str);

struct Token
{
	enum
	{
		OPEN_BRACE,
		CLOSE_BRACE,
		COLON,
		SQUARE_BRACKET_OPEN,
		SQUARE_BRACKET_CLOSE,
		COMMA,
		STRING,
		NUMBER,
		BOOL,
		UNKNOWN
	} type;

	int length;
	char* str;
};

struct JsonProp
{
	enum class Type : int
	{
		NUMBER,
		STRING,
		BOOL,
		OBJ,
		ARRAY
	};

	Type type;
	std::string name;

	union
	{
		double num;
		bool boolean;
		char* string;
		JsonArray array;
		Json* obj;
	};

	JsonProp() { }

	JsonProp(JsonProp& other) = delete;

	JsonProp(JsonProp&& other):
		type(other.type),
		name(std::move(other.name))
	{
		switch (other.type)
		{
			case Type::NUMBER: num = other.num; break;
			case Type::STRING:
				string = other.string;
				other.string = nullptr;
			break;
			case Type::BOOL: boolean = other.boolean; break;
			case Type::OBJ: 
				obj = other.obj;
				other.obj = nullptr;
			break;
			case Type::ARRAY: 
				array = other.array;
				other.array = { nullptr, 0 };
			break;
		}
	}

	~JsonProp()
	{
		switch (type)
		{
			case Type::STRING:
				delete string;
				string = nullptr;
			break;
			case Type::ARRAY:
				// TOOD: Verify Cleanup
				delete array.items;
				array.items = nullptr;
			break;
			case Type::OBJ:
				delete obj;
				obj = nullptr;
			break;
		}
	}
};


struct Json
{
	std::unordered_map<std::string, JsonProp> data;
};

static constexpr int getType(JsonProp::Type t) { return static_cast<std::underlying_type_t<JsonProp::Type>>(t); }

EXTERN_C Json * pureJSON_create()
{
	return new Json();
}

EXTERN_C void pureJSON_delete(Json* json)
{
	delete json;
}

EXTERN_C Json * pureJSON_parse(char * raw)
{
	char* itr = raw;
	Json* json = pureJSON_create();

	while (*itr != NULL)
	{
		if (isspace(*itr))
			itr = eatWhitespace(itr);

		if (!(*itr)) break;

		Token t = getToken(itr);

		itr = fillJson(itr, json);
	}

	return json;
}

EXTERN_C double pureJSON_getNum(Json * json, const char * propName)
{
	JsonProp& prop = json->data[std::string(propName)];
	assert(prop.type == JsonProp::Type::NUMBER);

	return prop.num;
}

EXTERN_C bool pureJSON_getBool(Json * json, const char * propName)
{
	JsonProp& prop = json->data[std::string(propName)];
	assert(prop.type == JsonProp::Type::BOOL);

	return prop.boolean;
}

EXTERN_C const char* pureJSON_getString(Json * json, const char * propName)
{
	JsonProp& prop = json->data[std::string(propName)];
	assert(prop.type == JsonProp::Type::STRING);

	return prop.string;
}

EXTERN_C JsonArray pureJSON_getArray(Json * json, const char * propName)
{
	JsonProp& prop = json->data[std::string(propName)];
	assert(prop.type == JsonProp::Type::ARRAY);

	return prop.array;
}

EXTERN_C Json* pureJSON_getObj(Json * json, const char * propName)
{
	JsonProp& prop = json->data[std::string(propName)];
	assert(prop.type == JsonProp::Type::OBJ);

	return prop.obj;
}

char* fillJson(char * raw, Json * json)
{
	char* itr = ++raw;

	bool done = false;

	while (!done)
	{
		itr = eatWhitespace(itr);

		Token t = getToken(itr);

		if (t.type == Token::STRING)
		{
			itr += t.length;
			itr = eatWhitespace(itr);
			Token colon = getToken(itr);

			if (colon.type == Token::COLON)
			{
				itr += colon.length;
				itr = eatWhitespace(itr);

				std::string propName = parseString(t.str);
				Token val = getToken(itr);

				JsonProp jprop = {};
				jprop.name = propName;

				switch (val.type)
				{
				case Token::BOOL:
					jprop.type = JsonProp::Type::BOOL;

					if (cmpSubStr(val.str, "false", val.length))
					{
						jprop.boolean = false;
					}
					else if (cmpSubStr(val.str, "true", val.length))
					{
						jprop.boolean = true;
					}

				break;
				case Token::NUMBER:
					jprop.type = JsonProp::Type::NUMBER;
					char buffer[255];
					memcpy(buffer, val.str, val.length);
					jprop.num = atof(buffer);
				break;
				case Token::STRING: {
					jprop.type = JsonProp::Type::STRING;
					jprop.string = parseCString(val.str);
				} break;
				case Token::SQUARE_BRACKET_OPEN:
					// TODO: Parse arrays
				break;
				case Token::OPEN_BRACE:
					jprop.type = JsonProp::Type::OBJ;
					jprop.obj = pureJSON_create();
					itr = fillJson(itr, jprop.obj);
				break;
				}

				json->data.emplace(propName, std::move(jprop));
				itr += val.length;

				itr = eatWhitespace(itr);
				
				Token next = getToken(itr);

				if (next.type == Token::COMMA)
				{
					itr += next.length;
					continue;
				}
				else
				{
					itr = eatWhitespace(itr);
					Token endBrace = getToken(itr);
					if (endBrace.type == Token::CLOSE_BRACE)
					{
						itr += endBrace.length;
						done = true;
					}
					else
					{
						std::cerr << "Missing comma after property name" << std::endl;
						return nullptr; // TODO: ERROR
					}
				}				

			}
			else
			{
				std::cerr << "Expected Colon after property name" << std::endl;
				return nullptr; // TODO: ERROR
			}
		}

	}
	return itr;
}

void snipDblQuotes(char* str, char *& begin, char *& end)
{
	begin = ++str;
	end = str;

	while (*end != '"')
	{
		if (*end == NULL)
		{
			std::cerr << "Could not find end of string parsed string" << std::endl;
			// TODO: ERROR
			end--;
			break;
		}

		end++;
	}
}

std::string parseString(char * str)
{
	char* begin;
	char* end;

	snipDblQuotes(str, begin, end);

	std::string result(begin, end);
	return result;
}

char * parseCString(char * str)
{
	char* begin;
	char* end;

	snipDblQuotes(str, begin, end);

	const uint32_t size = end - begin;
	char* result = new char[size + 1];
	result[size] = NULL;
	memcpy(result, begin, size);

	return result;
}

char* eatWhitespace(char* str)
{
	char* itr = str;
	while (isspace(*itr) && *itr != NULL)
		++itr;
	return itr;
}

Token getToken(char * str)
{
	Token t = { };
	t.type = Token::UNKNOWN;

	char* itr = str;
	while (!isspace(*itr) && *itr != NULL)
	{
		switch (*itr)
		{
			case '{': 
				t.type = Token::OPEN_BRACE;
				t.length = 1;
				t.str = itr;
				return t;
			break;
			case '}':
				t.type = Token::CLOSE_BRACE;
				t.length = 1;
				t.str = itr;
				return t;
			break;
			case ':':
				t.type = Token::COLON;
				t.length = 1;
				t.str = itr;
				return t;
			break;
			case ',':
				t.type = Token::COMMA;
				t.length = 1;
				t.str = itr;
				return t;
			break;
			case '"':
				t.str = itr;
				++itr;
				t.length = 1;
				while (*itr != '"')
				{
					t.length++;
					itr++;

					if (*itr == NULL)
					{
						std::cerr << "Reached End of file. Missing closing \" for string literal." << std::endl;
						t.str = nullptr;
						t.length = 0;
						return t;
					}
				}
				t.length++;
				t.type = Token::STRING;
				return t;
			break;
			case '[':
				t.type = Token::SQUARE_BRACKET_OPEN;
				t.length = 1;
				t.str = itr;
				return t;
			break;
			case ']':
				t.type = Token::SQUARE_BRACKET_CLOSE;
				t.length = 1;
				t.str = itr;
				return t;
			break;
			default:
				if (isalpha(*itr))
				{
					if (int size = tryParseBoolToken(itr))
					{
						t.type = Token::BOOL;
						t.length = size;
						t.str = itr;
						return t;
					}

				}
				else if (isdigit(*itr) || *itr == '-')
				{
					t.type = Token::NUMBER;
					t.str = itr;

					while (isdigit(*itr) || *itr == '-' || *itr == 'e' || *itr == '.')
					{
						itr++;
						t.length++;
					}

					return t;
				}

				itr++;
		}
	}

	return t;
}

int tryParseBoolToken(char* str)
{
	constexpr const char* t = "true";
	constexpr const char* f = "false";

	if (cmpSubStr(str, t, strlen(t)))
	{
		char* end = (str + strlen(t));
		bool result = !isalnum(*end);

		return strlen(t);
	}
	else if (cmpSubStr(str, f, strlen(f)))
	{
		char* end = (str + strlen(f));
		bool result = !isalnum(*end);

		return strlen(f);
	}
	return 0;
}

bool cmpSubStr(const char * a, const char * b, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		if (a[i] != b[i]) return false;
	}

	return true;
}

