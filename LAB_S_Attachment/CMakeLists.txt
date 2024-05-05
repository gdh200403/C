cmake_minimum_required(VERSION 3.5)

project(lc3simulator)

set (CMAKE_CXX_STANDARD 17)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
#set(BOOST_ROOT "D:/CppBoost/boost_1_80_0")
#set(BOOST_LIBRARYDIR "D:/CppBoost/boost_1_80_0/stage/lib")
#set(BOOST_INCLUDEDIR "D:/CppBoost/boost_1_80_0/boost")
#set(CMAKE_PREFIX_PATH "D:/CppBoost/boost_1_80_0")
#set(CMAKE_PREFIX_PATH "D:/CppBoost/boost_1_80_0/boost")
#set(CMAKE_PREFIX_PATH "D:/CppBoost/boost_1_80_0/stage/lib")
find_package(Boost 1.80.0 COMPONENTS program_options)


set(SOURCES
    src/main.cpp
    src/memory.cpp
    src/register.cpp
    src/simulator.cpp)

add_executable(lc3simulator ${SOURCES})

target_include_directories(lc3simulator
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

#include_directories("D:/CppBoost/boost_1_80_0")
#link_directories("D:/CppBoost/boost_1_80_0/stage/lib")
#link_libraries(Boost::program_options)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(lc3simulator ${Boost_LIBRARIES})
endif()
