cmake_minimum_required(VERSION 3.10)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(driver_leapify)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)

add_library(${PROJECT_NAME} SHARED
	${SRC_DIR}/main.cpp
	${SRC_DIR}/TrackedDeviceProvider.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${EXTERNAL_DIR}/openvr/headers)