cmake_minimum_required(VERSION 3.10)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(driver_leapify)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(VENDOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor)

add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

if (UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif (UNIX)

add_library(${PROJECT_NAME} SHARED
	${SRC_DIR}/main.cpp
	${SRC_DIR}/TrackedDeviceProvider.cpp
    ${SRC_DIR}/InterfaceHook.cpp
	${SRC_DIR}/TrackedController.cpp
	${SRC_DIR}/LeapConnection.cpp
	${SRC_DIR}/SimpleKalmanFilter.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${VENDOR_DIR}/LeapSDK/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${EXTERNAL_DIR}/openvr/headers)
target_include_directories(${PROJECT_NAME} PRIVATE ${EXTERNAL_DIR}/rcmp/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${EXTERNAL_DIR}/glm)

target_link_directories(${PROJECT_NAME} PRIVATE ${VENDOR_DIR}/LeapSDK/lib/x64)

add_subdirectory(${EXTERNAL_DIR}/rcmp)

target_link_libraries(${PROJECT_NAME} PRIVATE rcmp LeapC)