cmake_minimum_required(VERSION 3.5)

project(bcdice-irc-qt LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)

set(TS_FILES
        translations/bcdice-irc-qt_ja_JP.ts
)

set(QRC_FILES
        translations.qrc
)

set(SRC_FILES
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        src/mainwindow.ui
)

set(PROJECT_SOURCES
        ${SRC_FILES}
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR}/translations ${SRC_FILES} ${TS_FILES})

    qt_add_executable(bcdice-irc-qt
        ${PROJECT_SOURCES}
        ${QM_FILES}
        ${QRC_FILES}
    )
else()
    add_executable(bcdice-irc-qt
        ${PROJECT_SOURCES}
    )

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(bcdice-irc-qt PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
