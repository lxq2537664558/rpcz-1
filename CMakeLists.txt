project (Rpcz)

cmake_minimum_required (VERSION 2.8.3)

# CMake 2.8.3 supports solution folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_MODULE_PATH ${Rpcz_SOURCE_DIR}/cmake_modules)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()
message("-- Build type: ${CMAKE_BUILD_TYPE}")

if(MSVC OR BORLAND)
  # Boost automatic linking typically requests static libraries.
  set(Boost_USE_STATIC_LIBS ON)
endif()

option(rpcz_build_tests "Build rpcz's tests." OFF)
option(rpcz_build_examples "Build rpcz's examples." OFF)

find_package(ProtobufPlugin REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread program_options chrono date_time system)
find_package(ZeroMQ REQUIRED)
find_package(TBB REQUIRED)

find_path(CPPZMQ_DIR
  NAMES zmq.hpp
  PATHS ${ZeroMQ_INCLUDE_DIRS} ${CMAKE_INCLUDE_PATH}
)

include_directories(${ZeroMQ_INCLUDE_DIRS})
include_directories(${CPPZMQ_DIR})
include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${Rpcz_SOURCE_DIR}/include)

if(NOT MSVC)
set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-sign-compare -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -Werror")
endif(NOT MSVC)

set(CMAKE_OSX_ARCHITECTURES x86_64)

file(GLOB RPCZ_PUBLIC_HEADERS include/rpcz/*.hpp)

add_subdirectory(src)

if(rpcz_build_tests)
  enable_testing()
  add_subdirectory(test)
endif(rpcz_build_tests)

if(rpcz_build_examples)
  add_subdirectory(examples/cpp)
endif(rpcz_build_examples)

message(STATUS ${RPCZ_PUBLIC_HEADERS})
install(FILES ${RPCZ_PUBLIC_HEADERS} DESTINATION include/rpcz)
