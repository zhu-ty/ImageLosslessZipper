
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
