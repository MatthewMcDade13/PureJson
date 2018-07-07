#include "Tokenizer.h"
#include "Util.h"
#include <iostream>

static constexpr Token unknownToken()
{
	Token t = {};
	t.type = Token::UNKNOWN;
	t.length = 0;
	t.str = nullptr;
	return t;
}

static constexpr Token EOFToken()
{
	Token t = {};
	t.type = Token::JSON_EOF;
	t.length = 1;
	t.str = 0;
	return t;
}

static Token parseNumToken(const char* str);
static Token parseStringToken(const char* str);
static Token parseAlNumLiteralToken(const char* str);

void eatWhitespace(Cursor& cursor)
{
	while (isspace(*cursor.at) && *cursor.at != NULL)
		++cursor.at;
}


Token getToken(Cursor& cursor)
{
	Token t = {};
	t.length = 1;

	eatWhitespace(cursor);

	if (*cursor.at == NULL) return EOFToken();

	t.str = cursor.at;

	switch (*cursor.at)
	{
		case '{': t.type = Token::OPEN_BRACE;           break;
		case '}': t.type = Token::CLOSE_BRACE;          break;
		case ':': t.type = Token::COLON;			    break;
		case ',': t.type = Token::COMMA;			    break;
		case '[': t.type = Token::SQUARE_BRACKET_OPEN;  break;
		case ']': t.type = Token::SQUARE_BRACKET_CLOSE; break;
		case 'f':
		case 't':
		case 'n':
			t = parseAlNumLiteralToken(cursor.at);			
		break;
		case '"':
			t = parseStringToken(cursor.at);
		break;
		default:
			if (isdigit(*cursor.at) || *cursor.at == '-')
			{
				t = parseNumToken(cursor.at);
			}
			else
			{
				return unknownToken();
			}
	}

	cursor.at += t.length;
	return t;
}


Token parseNumToken(const char * str)
{
	Token t = {};
	t.type = Token::NUMBER;
	t.str = str;

	while (isdigit(*str) || *str == '-' || *str == 'e' || *str == '.')
	{
		str++;
		t.length++;
	}

	return t;
}

Token parseStringToken(const char * str)
{
	Token t = { };
	t.length = 1;
	t.str = str;
	++str;

	while (*str != '"')
	{
		t.length++;
		str++;

		if (*str == NULL)
		{
			std::cerr << "Reached End of file. Missing closing \" for string literal." << std::endl;
			return unknownToken();
		}
	}

	t.length++;
	t.type = Token::STRING;
	return t;
}

Token parseAlNumLiteralToken(const char * str)
{
	constexpr const char* t = "true";
	constexpr const char* f = "false";
	constexpr const char* n = "null";

	Token token = {};
	token.str = str;

	if (cmpSubStr(str, t, strlen(t)))
	{
		token.length = strlen(t);
		token.type = Token::BOOL;
	}
	else if (cmpSubStr(str, f, strlen(f)))
	{
		token.length = strlen(f);
		token.type = Token::BOOL;
	}
	else if (cmpSubStr(str, n, strlen(n)))
	{
		token.length = strlen(n);
		token.type = Token::JSON_NULL;
	}
	else
	{
		token = unknownToken();
	}


	return token;
}

