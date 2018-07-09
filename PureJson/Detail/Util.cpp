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


bool cmpSubStr(const char * left, const char * right, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		if (left[i] != right[i]) return false;
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

char * concatString(const char * left, const char * right)
{
	const size_t resultSize = strlen(left) + strlen(right);

	char* result = new char[resultSize + 1]();

	strcpy(result, left);
	strcat(result, right);

	return result;
}

void moveConcatString(char *& left, const char * right)
{
	char* result = concatString(left, right);
	delete[] left;
	left = result;
}

void moveConcatPropName(char *& left, const char * propName)
{
	constexpr const char* propEnd = "\": ";
	constexpr size_t endSize = sizeof(propEnd - 1);

	const size_t nameSize = strlen(propName);

	char* result = new char[nameSize + endSize + 2]();
	result[0] = '"';
	strcpy(result + 1, propName);
	strcpy(result + nameSize + 1, propEnd);

	moveConcatString(left, result);
}

void moveConcatStringLiteral(char *& left, const char * stringVal)
{
	const size_t strSize = strlen(stringVal);
	char* result = new char[strSize + 3]();
	result[0] = '"';

	strncpy(result + 1, stringVal, strSize);

	result[strSize + 1] = '"';

	moveConcatString(left, result);
}



