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
	//std::stringstream ss;

	//ss << "{\n";
	//ss << "\t\"prop\": true,\n";
	//ss << "\t\"prop2\": 123\n";
	//ss << "}";
	//std::string json = ss.str();

	//pureJSON_parse(json.c_str());S

	FILE *f = fopen("test.json", "rb");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);  //same as rewind(f);

	char *json = (char*)malloc(fsize + 1);
	fread(json, fsize, 1, f);
	fclose(f);

	json[fsize] = 0;

	pureJSON_parse(json);

	free(json);

    return 0;
}

