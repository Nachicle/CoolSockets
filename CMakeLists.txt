cmake_minimum_required(VERSION 3.0.0)
cmake_policy(SET CMP0048 NEW)

project(CoolSockets VERSION 0.1.0)

include_directories(include/)

if(UNIX) 
    message(STATUS "Using Linux sources")
    set(SOURCE 
        source/linux/CoolSockets.c
    )
endif()

if(WIN32)
    message(STATUS "Using Windows sources")
    set(SOURCE 
        source/windows/CoolSockets.c
    )
endif()

add_executable(test Main.c ${SOURCE})
