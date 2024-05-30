cmake_minimum_required(VERSION 3.10)

if (POLICY CMP0141) # MSVC for Windows support
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(driver_leap)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(VENDOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor)

add_definitions(-DGLM_ENABLE_EXPERIMENTAL) # Enable GLM's GTX functions

add_library(${PROJECT_NAME} SHARED
	${SRC_DIR}/main.cpp
	${SRC_DIR}/core/CServerDriver.cpp
	${SRC_DIR}/controller/CLeapIndexController.cpp
	${SRC_DIR}/controller/CControllerButton.cpp
	${SRC_DIR}/controller/CJoyconInput.cpp
	${SRC_DIR}/leap/CLeapFrame.cpp
	${SRC_DIR}/leap/CLeapHand.cpp
	${SRC_DIR}/leap/CLeapPoller.cpp
	${SRC_DIR}/utils/Utils.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE ${VENDOR_DIR}/openvr/headers)
target_include_directories(${PROJECT_NAME} PRIVATE ${VENDOR_DIR}/glm)

if (WIN32)
	target_include_directories(${PROJECT_NAME} PRIVATE ${VENDOR_DIR}/LeapSDK/include)
	target_link_directories(${PROJECT_NAME} PRIVATE ${VENDOR_DIR}/LeapSDK/lib/x64)
else()
	target_include_directories(${PROJECT_NAME} PRIVATE ${VENDOR_DIR}/LeapSDK_Linux/include)
	target_link_directories(${PROJECT_NAME} PRIVATE ${VENDOR_DIR}/LeapSDK_Linux/lib/x64)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${VENDOR_DIR}/JSL)
target_link_directories(${PROJECT_NAME} PRIVATE ${VENDOR_DIR}/JSL/x64)

target_link_libraries(${PROJECT_NAME} PRIVATE LeapC JoyShockLibrary)

add_custom_command(TARGET driver_leap
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:driver_leap>"
        "C:/Program Files (x86)/Steam/steamapps/common/SteamVR/drivers/leap/bin/win64/$<TARGET_FILE_NAME:driver_leap>"
)