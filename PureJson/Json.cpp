#include "stdafx.h"
#include "Json.h"
#include <unordered_map>
#include <cctype>
#include <iostream>
#include <string>

static char* eatWhitespace(char* str);
static struct Token getToken(char* str);
static int tryParseBoolToken(char* str);
static bool cmpSubStr(const char* a, const char* b, size_t length);
static char* fillJson(char* raw, Json* json);


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
		NUMBER_DBL,
		NUMBER_INT,
		STRING,
		BOOL,
		OBJ,
		ARRAY
	};

	Type type;
	std::string name;

	union
	{
		double dnum;
		bool flag;
		long long inum;
		std::string string;
		Json** array;
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
			case Type::NUMBER_DBL: dnum = other.dnum; break;
			case Type::NUMBER_INT: inum = other.inum; break;
			case Type::STRING: string = std::move(other.string); break;
			case Type::BOOL: flag = other.flag; break;
			case Type::OBJ: 
				obj = other.obj;
				other.obj = nullptr;
			break;
			case Type::ARRAY: 
				array = other.array;
				other.array = nullptr;
			break;
		}
	}

	~JsonProp()
	{
		if (type == Type::STRING)
			string.~basic_string();
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

		switch (t.type)
		{
			case Token::OPEN_BRACE:

			break;
		}

		if (t.type != Token::UNKNOWN)
		{
			char* str = (char*)malloc(t.length + 1);

			memcpy(str, itr, t.length);
			str[t.length] = NULL;

			std::cout << str << std::endl;

			itr += t.length;

			free(str);
		}
		else
		{
			itr++;
		}
	}

	return json;
}


char* fillJson(char * raw, Json * json)
{
	char* itr = ++raw;

	Token t = getToken(itr);
	if (t.type == Token::UNKNOWN) return nullptr; // TODO: error 

	if (t.type == Token::CLOSE_BRACE) return itr;

	if (t.type == Token::STRING)
	{
		itr += t.length;
		Token colon = getToken(t.str + t.length);
		if (colon.type == Token::COLON)
		{
			itr += colon.length;
			std::string propName(t.str, t.length);
			Token val = getToken(itr);

			switch (val.type)
			{
				case Token::BOOL: {
					JsonProp jprop = {};
					jprop.name = propName;
					jprop.type = JsonProp::Type::BOOL;

					if (cmpSubStr(val.str, "false", val.length))
					{
						jprop.flag = false;
						json->data.emplace(propName, std::move(jprop));

					}
					else if (cmpSubStr(val.str, "true", val.length))
					{
						jprop.flag = true;
						json->data.emplace(propName, std::move(jprop));

					}
				} break;
				case Token::NUMBER:
				break;
				case Token::STRING:
				break;
			}
		}
		else
		{
			// TODO: error 
		}
	}
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
						exit(1);
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

