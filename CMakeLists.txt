cmake_minimum_required(VERSION 3.0.0)
cmake_policy(SET CMP0048 NEW)

project(CoolSockets VERSION 0.1.0)

include_directories(include/)

if(UNIX) 
    message(STATUS "### COOLSOCKETS: USING LINUX SOURCES ###")
    set(SOURCE 
        source/linux/CoolSockets.c
    )
endif()

if(WIN32)
    message(STATUS "### COOLSOCKETS: USING WINDOWS SOURCES ###")
    set(SOURCE 
        source/windows/CoolSockets.c
    )
endif()

add_executable(test Main.c ${SOURCE})

if(UNIX)

endif()

if(WIN32)
    target_link_libraries(test wsock32 ws2_32)
endif()