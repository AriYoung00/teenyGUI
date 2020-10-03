cmake_minimum_required(VERSION 3.17)
project(microGUI)

set(CMAKE_CXX_STANDARD 14)

file(GLOB microGUI_SRC
        "include/*.h"
        "src/*.cpp"
        )

add_executable(microGUI main.cpp ${microGUI_SRC})
target_include_directories(microGUI PRIVATE "include")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake-modules")
find_package(SFML REQUIRED system window graphics network audio)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(microGUI ${SFML_LIBRARIES})
endif()