cmake_minimum_required(VERSION 2.8)

project(AQDisplay)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE})
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

# set(Boost_DEBUG 1)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.48.0 COMPONENTS program_options filesystem system REQUIRED)

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${Boost_INCLUDE_DIRS}
)

link_directories(
  ${LIBRARY_OUTPUT_PATH}
  ${Boost_LIBRARY_DIRS}
)

# Linux Specific code
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(OperatingSystem "Linux")
  set(CMAKE_CXX_FLAGS "-O0 -ggdb -std=c++0x")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# Mac OS X Specific code
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(OperatingSystem "Darwin")
  set(CMAKE_CXX_FLAGS "-O0 -ggdb -std=c++11 -stdlib=libc++ -Wall -Werror -DENABLE_AQL_PARSER")
  set(CMAKE_SHARED_LINKER_FLAGS "-lc++")
  set(CMAKE_MODULE_LINKER_FLAGS "-lc++")
  set(CMAKE_EXE_LINKER_FLAGS "-lc++")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

# source files
file(GLOB aq_util_source_files aq/util/*.cpp aq/util/*.h)
file(GLOB aq_engine_source_files aq/engine/*.cpp aq/engine/*.h)
file(GLOB aq_display_source_files aq/display/*.cpp aq/display/*.h)

# static library
add_library(aq-util STATIC ${aq_util_source_files})
add_library(aq-engine STATIC ${aq_engine_source_files})
add_library(aq-display STATIC ${aq_display_source_files})

# # shared library
# add_library(aq-util-shared SHARED ${aq_util_source_files})
# target_link_libraries(aq-util-shared ${Boost_LIBRARIES})
# set_target_properties(aq-util-shared PROPERTIES OUTPUT_NAME "aq-util")
# 
# add_library(aq-engine-shared SHARED ${aq_engine_source_files})
# target_link_libraries(aq-engine-shared ${Boost_LIBRARIES} aq-util-shared)
# set_target_properties(aq-engine-shared PROPERTIES OUTPUT_NAME "aq-engine")
# 
# add_library(aq-display-shared SHARED ${aq_display_source_files})
# target_link_libraries(aq-display-shared ${Boost_LIBRARIES} aq-util-shared aq-engine-shared)
# set_target_properties(aq-display-shared PROPERTIES OUTPUT_NAME "aq-display")

# aql-parser
add_executable(aql-parser aq/aql_parser.cpp)
target_link_libraries(aql-parser aq-util ${Boost_LIBRARIES})

# aq-display
add_executable(aq aq/aq.cpp)
target_link_libraries(aq aq-display aq-engine aq-util ${Boost_LIBRARIES})

