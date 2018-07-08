#include "Util.h"
#include <iostream>

void snipDblQuotes(const char* str, const char *& begin, const char *& end)
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

std::string parseString(const char * str)
{
	const char* begin;
	const char* end;

	snipDblQuotes(str, begin, end);

	std::string result(begin, end);
	return result;
}

char * parseCString(const char * str)
{
	const char* begin;
	const char* end;

	snipDblQuotes(str, begin, end);

	const uint32_t size = end - begin;
	char* result = new char[size + 1];
	result[size] = NULL;
	memcpy(result, begin, size);

	return result;
}


bool cmpSubStr(const char * a, const char * b, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		if (a[i] != b[i]) return false;
	}

	return true;
}


char * cpyStringDynamic(const char * str)
{
	const uint32_t stringLen = strlen(str);
	char* result = new char[stringLen + 1]();
	strcpy(result, str);

	return result;
}



