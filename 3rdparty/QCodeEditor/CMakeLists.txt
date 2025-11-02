cmake_minimum_required(VERSION 3.10)
project(QCodeEditor LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find includes in corresponding build directories
find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(RESOURCES_FILE resources/qcodeeditor_resources.qrc)

set(INCLUDE_FILES
    include/QHighlightRule.hpp
    include/QHighlightBlockRule.hpp
    include/QCodeEditor.hpp
    include/QLineNumberArea.hpp
    include/QStyleSyntaxHighlighter.hpp
    include/QSyntaxStyle.hpp
    include/QLanguage.hpp
    include/QFramedTextAttribute.hpp)

set(SOURCE_FILES
    src/QCodeEditor.cpp src/QLineNumberArea.cpp src/QSyntaxStyle.cpp
    src/QStyleSyntaxHighlighter.cpp src/QLanguage.cpp
    src/QFramedTextAttribute.cpp)

add_library(QCodeEditor STATIC ${RESOURCES_FILE} ${SOURCE_FILES}
                               ${INCLUDE_FILES})

target_include_directories(QCodeEditor PUBLIC include)

target_link_libraries(QCodeEditor PRIVATE Qt6::Widgets)
