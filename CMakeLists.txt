cmake_minimum_required(VERSION 3.16)
project(QtMonopoly VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Automatically run MOC, RCC, and UIC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 modules
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Add your source files here as the project grows
set(PROJECT_SOURCES
    src/main.cpp
)

add_executable(QtMonopoly ${PROJECT_SOURCES})

target_link_libraries(QtMonopoly PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)