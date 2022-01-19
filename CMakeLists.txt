cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0048 NEW)

project(CoolSockets VERSION 0.0.1)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/lib)

if(UNIX) 
    message(STATUS "### COOLSOCKETS: USING LINUX SOURCES ###")
    set(PLATFORM_SOURCES linux)
endif()

if(WIN32)
    message(STATUS "### COOLSOCKETS: USING WINDOWS SOURCES ###")
    set(PLATFORM_SOURCES windows)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()

include_directories(include/)
set(SOURCES 
    source/${PLATFORM_SOURCES}/CoolSockets.c
)

add_executable(Server examples/Server.c ${SOURCES})
add_executable(Client examples/Client.c ${SOURCES})
add_library(${PROJECT_NAME} SHARED ${SOURCES})

if(UNIX)

endif()

if(WIN32)
    target_link_libraries(Server wsock32 ws2_32)
    target_link_libraries(Client wsock32 ws2_32)
    target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
endif()
