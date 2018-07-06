// JsonMain.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#pragma warning(disable : 4996)
#include "Json.h"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

int main()
{
	FILE *f = fopen("test.json", "rb");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);  

	char *rawJson = (char*)malloc(fsize + 1);
	fread(rawJson, fsize, 1, f);
	fclose(f);

	rawJson[fsize] = 0;

	Json* json = pureJSON_parse(rawJson);

	bool p1 = pureJSON_getBool(json, "prop");
	double p2 = pureJSON_getNum(json, "prop2");
	Json* subObj = pureJSON_getObj(json, "prop3");

	const char* p3name = pureJSON_getString(subObj, "name");
	double p3age = pureJSON_getNum(subObj, "age");

	free(rawJson);
	pureJSON_delete(json);

    return 0;
}

