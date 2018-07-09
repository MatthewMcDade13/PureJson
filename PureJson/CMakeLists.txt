cmake_minimum_required(VERSION 3.10)
project(PureJSON)

set(CMAKE_CXX_STANDARD 17)

target_include_directories(PureJSON "Debug")

add_library(
    PureJSON 
    SHARED 
    "dllmain.cpp"
    "stdafx.cpp"
    "stdafx.h"
    "targetver.h"
    "Tokenizer.cpp" 
    "Tokenizer.h" 
    "Util.cpp"
    "Util.h"
    "Define.h"
    "Json.cpp"
    "Json.h"
)

install(
    FILES
    "Json.h"
    "Define.h"
    DESTINATION "include/${PROJECT_NAME}"
)

install(FILES "${PROJECT_BINARY_DIR}/purejson-config.cmake" DESTINATION lib/cmake/${PROJECT_NAME})
