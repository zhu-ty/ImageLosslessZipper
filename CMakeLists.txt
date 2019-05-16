
message(
  "
           
               ┌─┐       ┌─┐
            ┌──┘ ┴───────┘ ┴──┐
            │                 │
            │       ───       │
            │  ─┬┘       └┬─  │
            │                 │
            │       ─┴─       │
            │                 │
            └───┐         ┌───┘
                │         │
                │         │
                │         │
                │         └──────────────┐
                │                        │
                │                        ├─┐
                │                        ┌─┘    
                │                        │
                └─┐  ┐  ┌───────┬──┐  ┌──┘         
                  │ ─┤ ─┤       │ ─┤ ─┤         
                  └──┴──┘       └──┴──┘ 
                With the bless of the legend,
                there shall not be any bug.
      
  ")

cmake_minimum_required (VERSION 3.10)
set(name "ImageLosslessZipper")
project ( ${name} )
# cmake module
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
# c++ version
set (CMAKE_CXX_STANDARD 11)
set (WIN_LIB_PATH "E:/libs")
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

#OpenCV
if ( WIN32 )
    set(OpenCV_DIR ${WIN_LIB_PATH}/OpenCV/x64/vc14/lib)
    set(OpenCV_CONFIG_PATH ${WIN_LIB_PATH}/OpenCV)
endif()
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

#MainFolder
file (GLOB this_headers_h "./*.h")
file (GLOB this_headers_hpp "./*.hpp")
set (MY_HEADER_FILES
	${this_headers_h}
	${this_headers_hpp}
)
file (GLOB this_headers_cpp "./*.cpp")
file (GLOB this_headers_c "./*.c")
set (MY_SOURCE_FILES
    ${this_headers_cpp}
    ${this_headers_c}
)
include_directories(
    ${PROJECT_SOURCE_DIR}
)

#Link&Exec
add_executable(${name}
    ${MY_HEADER_FILES}
    ${MY_SOURCE_FILES}
    )
target_link_libraries(${name}
    ${OpenCV_LIBS}
    )


SET(MAJOR_VERSION "1")
SET(MINOR_VERSION "0")
add_definitions( -D__MAJOR_VERSION__=${MAJOR_VERSION})
add_definitions( -D__MINOR_VERSION__=${MINOR_VERSION})

FILE(WRITE ${CMAKE_BINARY_DIR}/version.h.in
"\#define __GIT_VERSION__ \"@VERSION@\"\n"
)
IF(EXISTS ${PROJECT_SOURCE_DIR}/.git)
FILE(WRITE ${CMAKE_BINARY_DIR}/version.cmake
"EXECUTE_PROCESS(
     COMMAND git log -1 --format=%ad --date=format:%y%m%d.%H%M%S
     OUTPUT_VARIABLE VERSION
     OUTPUT_STRIP_TRAILING_WHITESPACE
 )
 CONFIGURE_FILE(\${SRC} \${DST} @ONLY)
")
ELSE()
FILE(WRITE ${CMAKE_BINARY_DIR}/version.cmake
"EXECUTE_PROCESS(
     COMMAND echo unknown
     OUTPUT_VARIABLE VERSION
     OUTPUT_STRIP_TRAILING_WHITESPACE
 )
 CONFIGURE_FILE(\${SRC} \${DST} @ONLY)
")
ENDIF()
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})
#ADD_EXECUTABLE(main main.c)
ADD_CUSTOM_TARGET(
    version
    ${CMAKE_COMMAND} -D SRC=${CMAKE_BINARY_DIR}/version.h.in
                     -D DST=${CMAKE_BINARY_DIR}/version.h
                     -P ${CMAKE_BINARY_DIR}/version.cmake
)

ADD_DEPENDENCIES(${name} version)