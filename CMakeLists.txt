cmake_minimum_required(VERSION 2.8)
project (unique.moe)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
find_package(Threads)
find_package(OpenSSL)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++1y -pedantic")
find_package( Boost 1.63 COMPONENTS system thread REQUIRED )

include_directories( ${Boost_INCLUDE_DIR} )

set(PROJECT_INCLUDE_DIR 
${PROJECT_SOURCE_DIR}/include
)

include_directories("${PROJECT_INCLUDE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")

link_directories(/usr/local/lib)

add_executable(uniquehttp2 index.cpp)
target_link_libraries(uniquehttp2 ${Boost_LIBRARIES})
target_link_libraries(uniquehttp2 ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(uniquehttp2 nghttp2_asio)
target_link_libraries(uniquehttp2 ssl)
target_link_libraries(uniquehttp2 crypto)
