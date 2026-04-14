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

# Automatically find all source and header files in the src directory
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/*.h"
)
add_custom_target(format
    COMMAND clang-format -i ${ALL_SOURCE_FILES}
    COMMENT "Formating code with Clang-Format"
)


add_executable(QtMonopoly ${PROJECT_SOURCES} resources.qrc)
# Tell the compiler where to look for header files
target_include_directories(QtMonopoly PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
)

target_link_libraries(QtMonopoly PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

target_link_libraries(QtMonopoly PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)