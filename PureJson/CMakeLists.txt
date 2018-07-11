cmake_minimum_required(VERSION 3.10)
project(PureJSON)

set(CMAKE_CXX_STANDARD 17)


add_library(
        PureJSON
        SHARED
        "dllmain.cpp"
        "stdafx.cpp"
        "stdafx.h"
        "targetver.h"
        "Detail/Tokenizer.cpp"
        "Detail/Tokenizer.h"
        "Detail/Util.cpp"
        "Detail/Util.h"
        "Define.h"
        "Json.cpp"
        "Json.h"
)

target_include_directories(PureJSON PRIVATE "Detail")

install(
        FILES
        "Json.h"
        "Define.h"
        DESTINATION "include/${PROJECT_NAME}"
)

configure_file(purejson-config.cmake.in "${PROJECT_BINARY_DIR}/purejson-config.cmake")

install(FILES "${PROJECT_BINARY_DIR}/purejson-config.cmake" DESTINATION lib/cmake/${PROJECT_NAME})
