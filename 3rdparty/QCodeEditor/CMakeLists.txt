cmake_minimum_required(VERSION 3.6)
project(QCodeEditor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

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

# Create code for QObjects
set(CMAKE_AUTOMOC ON)

# Create code from resource files
set(CMAKE_AUTORCC ON)

# Find includes in corresponding build directories
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_library(QCodeEditor STATIC ${RESOURCES_FILE} ${SOURCE_FILES}
                               ${INCLUDE_FILES})

target_include_directories(QCodeEditor PUBLIC include)

if(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(
        QCodeEditor
        PRIVATE -ansi
                -pedantic
                -Wall
                -Wextra
                -Weffc++
                -Woverloaded-virtual
                -Winit-self
                -Wunreachable-code)
endif(CMAKE_COMPILER_IS_GNUCXX)

target_link_libraries(QCodeEditor PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
