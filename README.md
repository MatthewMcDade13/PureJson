# Pure Json

Lightweight single header C/C++ json parser. Was mainly made for educational purposes and playing around with parsing.
If you need anything robust, this is definitely not the library for you.


Installing
===========

* Copy and paste or download PureJson.h and make a cpp file for it.
* in created cpp file:
    ```cpp
    #define PURE_JSON_IMPLEMENTATION
    #include "PureJson.h"
    ```
* Done!


Usage
======

```cpp
#include <iostream>
#include <fstream>
#include "Json.h"

int main()
{
	std::ifstream ifs{ "config.json" };
	std::string jsonstr{ std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>() };

	// use pj_parseObj if root of json is an object, otherwise use pj_parseArray
	pj_Object* json = pj_parseObj(jsonstr.c_str());
	
	// There is also a simple RAII wrapper for the root object available if you so choose
	// pj::ObjectRoot json = pj_parseObj(jsonstr.c_str());

	double version = pj_objGetNum(json, "version");

	pj_objSetString(json, "root", "C:\\Programs\\Games");

	pj_Array* arr = pj_objGetArray(json, "colors");

	for (size_t i = 0; i < pj_getArraySize(arr); i++)
		std::cout << pj_arrayGetString(arr, i) << std::endl;

	std::cout << version << std::endl;

	// similar to parsing, if root is object use this function, otherwise use pj_arrayToFile
	pj_objToFile(json, true, "outfile.json");

	// only need to release the root object
	pj_deleteObj(json);
}
```

 There is also somewhat of a test/example in JsonMain directory
 
 Inspired by Casey Muratori's youtube video on parsing: https://www.youtube.com/watch?v=Ha3NbEhXAtU
 
