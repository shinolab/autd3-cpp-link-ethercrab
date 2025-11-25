cmake_minimum_required(VERSION 3.21)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt. ")
endif()

project(autd3-examples)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(AUTD_LOCAL_TEST "For local test build" OFF)

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.24.0") 
  cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)
if(AUTD_LOCAL_TEST)
  FetchContent_Declare(
    autd3_link_ethercrab
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/..
  )
elseif(WIN32)
  FetchContent_Declare(
    autd3_link_ethercrab
    URL https://github.com/shinolab/autd3-cpp-link-ethercrab/releases/download/v38.0.1/autd3-link-ethercrab-v38.0.1-win-x64.zip
  )
elseif(APPLE)
  FetchContent_Declare(
    autd3_link_ethercrab
    URL https://github.com/shinolab/autd3-cpp-link-ethercrab/releases/download/v38.0.1/autd3-link-ethercrab-v38.0.1-macos-aarch64.tar.gz
  )
else()
  FetchContent_Declare(
    autd3_link_ethercrab
    URL https://github.com/shinolab/autd3-cpp-link-ethercrab/releases/download/v38.0.1/autd3-link-ethercrab-v38.0.1-linux-x64.tar.gz
  )
endif()
FetchContent_MakeAvailable(autd3_link_ethercrab)

add_executable(ethercrab ethercrab.cpp)
target_link_libraries(ethercrab PRIVATE autd3::autd3 autd3::link::ethercrab)
