#pragma once

struct Token
{
	enum Type
	{
		UNKNOWN,
		OPEN_BRACE,
		CLOSE_BRACE,
		COLON,
		SQUARE_BRACKET_OPEN,
		SQUARE_BRACKET_CLOSE,
		COMMA,
		STRING,
		NUMBER,
		BOOL,
		JSON_NULL,
		JSON_EOF
	} type;

	int length;
	const char* str;
};

struct Cursor
{
	const char* at;
};

void eatWhitespace(Cursor& cursor);

Token getToken(Cursor& cursor);
