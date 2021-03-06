// JsonMain.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#pragma warning(disable : 4996)
#include "../PureJson/PureJson.h"
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

	pj::ObjectRoot json = pj_parseObj(rawJson);

	bool p1 = pj_objGetBool(json.handle, "prop");
	double p2 = pj_objGetNum(json.handle, "prop2");
	pj_Object* subObj = pj_objGetObj(json.handle, "prop3");

	const char* p3name = pj_objGetString(subObj, "name");
	double p3age = pj_objGetNum(subObj, "age");

	pj_Array* arr = pj_objGetArray(json.handle, "prop4");
	std::string p4num1 = pj_arrayGetString(arr, 0);
	const char* p4num2 = pj_arrayGetString(arr, 1);
	const char* p4name3 = pj_arrayGetString(arr, 2);

	void* prop5null = pj_objGetObj(json.handle, "prop5");

	pj_arrayAddNull(arr);
	pj_objSetNull(json.handle, "itsnullfam");

	void* arrAddNull = pj_arrayGetObj(arr, 3);
	void* objSetNull = pj_objGetObj(json.handle, "itsnullfam");

	pj_objForEachKey(json.handle, [](pj_Object* obj, const char* key) {
		switch (pj_getObjPropType(obj, key))
		{
			case PJ_VALUE_NUMBER:
				std::cout << pj_objGetNum(obj, key) << std::endl;
			break;
			case PJ_VALUE_STRING:
				std::cout << pj_objGetString(obj, key) << std::endl;
			break;
			case PJ_VALUE_BOOL:
				std::cout << pj_objGetBool(obj, key) << std::endl;
			break;
			case PJ_VALUE_OBJ:
				std::cout << "OBJECT" << std::endl;
			break;
			case PJ_VALUE_ARRAY:
				std::cout << "ARRAY" << std::endl;
			break;
			case PJ_VALUE_NULL:
				std::cout << "NULL" << std::endl;
			break;
		}
	});

	for (size_t i = 0; i < pj_getArraySize(arr); i++)
	{
		const char* item = pj_arrayGetString(arr, i);
		if (item)
		{
			std::cout << item << std::endl;
		}
	}

	pj::String stringified = pj_objToString(json.handle, true);

	pj_objToFile(json.handle, true, "testout.json");

	free(rawJson);

    return 0;
}

