cmake_minimum_required(VERSION 3.21)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt. ")
endif()

project(autd3_link_ethercrab VERSION 38.0.0)

message(STATUS "Build autd3_link_ethercrab: ${PROJECT_VERSION}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

option(USE_SYSTEM_EIGEN "Use system installed eigen3" OFF)

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.24.0") 
  cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)
if(WIN32)
  FetchContent_Declare(
    autd3
    URL https://github.com/shinolab/autd3-cpp/releases/download/v38.0.0/autd3-v38.0.0-win-x64.zip
  )
elseif(APPLE)
  FetchContent_Declare(
    autd3
    URL https://github.com/shinolab/autd3-cpp/releases/download/v38.0.0/autd3-v38.0.0-macos-aarch64.tar.gz
  )
else()
  FetchContent_Declare(
    autd3
    URL https://github.com/shinolab/autd3-cpp/releases/download/v38.0.0/autd3-v38.0.0-linux-x64.tar.gz
  )
endif()
FetchContent_MakeAvailable(autd3)

macro(autd_target_link_libraries target lib)
  if(WIN32)
    target_link_libraries(${target} INTERFACE "${lib}.dll.lib")
  else()
    target_link_libraries(${target} INTERFACE ${lib})
  endif()
endmacro()

add_library(autd3_link_ethercrab INTERFACE)
target_include_directories(autd3_link_ethercrab INTERFACE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(autd3_link_ethercrab INTERFACE autd3::common)
autd_target_link_libraries(autd3_link_ethercrab autd3_capi_link_ethercrab)
if(WIN32)
  target_link_directories(autd3_link_ethercrab INTERFACE ${PROJECT_SOURCE_DIR}/lib)
else()
  target_link_directories(autd3_link_ethercrab INTERFACE ${PROJECT_SOURCE_DIR}/bin)
endif()
if(WIN32)
  target_link_libraries(autd3_link_ethercrab INTERFACE Ws2_32.lib Winmm.lib)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  add_custom_target(autd3_capi_link_ethercrab)
  add_custom_command(TARGET autd3_capi_link_ethercrab POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/$<CONFIG>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/bin/autd3_capi_link_ethercrab.dll" ${CMAKE_BINARY_DIR}/$<CONFIG>
    VERBATIM
  )
  set_property(TARGET autd3_capi_link_ethercrab PROPERTY FOLDER "autd3capi") 
  add_dependencies(autd3_link_ethercrab autd3_capi_link_ethercrab)
elseif(APPLE)
  target_link_libraries(autd3_link_ethercrab INTERFACE pcap)
endif()
add_library(autd3::link::ethercrab ALIAS autd3_link_ethercrab)
