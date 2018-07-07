#pragma once

#include <string>

void snipDblQuotes(const char* str, const char*& begin, const char*& end);

std::string parseString(const char* str);

char* parseCString(const char* str);

bool cmpSubStr(const char* a, const char* b, size_t length);