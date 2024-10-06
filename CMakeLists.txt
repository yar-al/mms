project(mms)

set(CMAKE_BINARY_DIR "./bin/")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "./lib/")

cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(Catch2 REQUIRED)

add_library(MemoryManagerInternal STATIC
        MemoryManager/MemoryElements.h
        MemoryManager/MemoryElements.cpp
        MemoryManager/MemorySpace.h
        MemoryManager/MemoryTable.h
        MemoryManager/MemoryTable.cpp
)

add_library(MemoryManager STATIC
        MemoryManager/MemoryManager.h
        MemoryManager/MemoryManager.cpp
)

add_executable(test
        Test/test.cpp
)

add_executable(ConsoleApplication
        Console/console.cpp
        Console/MemoryViewer.h
        Console/menu.h

)

target_link_libraries(MemoryManager MemoryManagerInternal)
target_link_libraries(test MemoryManager)
target_link_libraries(ConsoleApplication MemoryManager)