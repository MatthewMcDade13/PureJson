#pragma once

#include <string>

void snipDblQuotes(const char* str, const char*& begin, const char*& end);

std::string parseString(const char* str);

char* parseCString(const char* str);

bool cmpSubStr(const char* left, const char* right, size_t length);
char* cpyStringDynamic(const char* str);

char* concatString(const char* left, const char* right);
void moveConcatString(char*& left, const char* right);

void moveConcatPropName(char*& left, const char* propName);
void moveConcatStringLiteral(char*& left, const char* stringVal);