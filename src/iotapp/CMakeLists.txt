cmake_minimum_required(VERSION 3.1)
# note: cxx-17 requires cmake 3.8, cxx-20 requires cmake 3.12
project(iotapp CXX)

file(GLOB SRC_FILES
       "*.cpp"
       "*.h"
)

include_directories(
    ./
    ../
    ../../artifact/include)
link_directories(
    ../../artifact/lib)

add_executable(${PROJECT_NAME} ${SRC_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 14)

#set warnings
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX /wd4068)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif ()

target_link_libraries(${PROJECT_NAME} aws-c-common aws-crt-cpp)
