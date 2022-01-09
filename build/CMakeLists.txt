cmake_minimum_required(VERSION 3.9)


if (POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW) # CMake 3.0
endif ()

set(CMAKE_CXX_STANDARD 14)

set(PROJECT_NAME simple_http_server)

project(${PROJECT_NAME})

set(PROJECT_DIR "..")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../${PROJECT_DIR}/bin/)


if (${CMAKE_GENERATOR} STRGREATER "Unix")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif ()



set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_VERBOSE_MAKEFILE On)
set(CMAKE_SUPPRESS_REGENERATION true) #disables ZERO_MAKE

file(GLOB_RECURSE HEADERS
        "${PROJECT_DIR}/include/*.h")
file(GLOB_RECURSE SOURCES
        "${PROJECT_DIR}/src/*.cpp")

source_group("include" FILES ${HEADERS})
source_group("src" FILES ${SOURCES})


include_directories(SYSTEM
        ${PROJECT_DIR}/include
        $ENV{BOOST_HOME}/include
        )


link_directories(
        $ENV{BOOST_HOME}/lib
)

message("header" ${HEADERS})
message("source" ${SOURCES} "${PROJECT_DIR}/src/")

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules")

target_link_libraries(${PROJECT_NAME}
        )
