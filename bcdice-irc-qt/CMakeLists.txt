cmake_minimum_required(VERSION 3.8)

project(bcdice-irc-qt LANGUAGES CXX)

set(APP_NAME "bcdice-irc-qt")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/grpc.cmake)

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

# Proto file
get_filename_component(bcdice_irc_proto "../bcdice-irc-proto/bcdice_irc.proto" ABSOLUTE)
get_filename_component(bcdice_irc_proto_path "${bcdice_irc_proto}" PATH)

# Generated sources
set(bcdice_irc_proto_srcs    "${CMAKE_CURRENT_BINARY_DIR}/bcdice_irc.pb.cc")
set(bcdice_irc_proto_headers "${CMAKE_CURRENT_BINARY_DIR}/bcdice_irc.pb.h")
set(bcdice_irc_grpc_srcs     "${CMAKE_CURRENT_BINARY_DIR}/bcdice_irc.grpc.pb.cc")
set(bcdice_irc_grpc_headers  "${CMAKE_CURRENT_BINARY_DIR}/bcdice_irc.grpc.pb.h")

foreach(_target
  "${bcdice_irc_proto_srcs}"
  "${bcdice_irc_proto_headers}"
  "${bcdice_irc_grpc_srcs}"
  "${bcdice_irc_grpc_headers}"
)
  set_property(SOURCE ${_target} PROPERTY SKIP_AUTOGEN ON)
endforeach()

add_custom_command(
  OUTPUT
    "${bcdice_irc_proto_srcs}"
    "${bcdice_irc_proto_headers}"
    "${bcdice_irc_grpc_srcs}"
    "${bcdice_irc_grpc_headers}"
  COMMAND ${_PROTOBUF_PROTOC}
  ARGS
    --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
    --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
    -I "${bcdice_irc_proto_path}"
    --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
    "${bcdice_irc_proto}"
  DEPENDS "${bcdice_irc_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_library(bcdice_irc_grpc_proto
  ${bcdice_irc_grpc_srcs}
  ${bcdice_irc_grpc_headers}
  ${bcdice_irc_proto_srcs}
  ${bcdice_irc_proto_headers})
target_link_libraries(bcdice_irc_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR}/translations ${SRC_FILES} ${TS_FILES})

    qt_add_executable(${APP_NAME}
        ${PROJECT_SOURCES}
        ${QM_FILES}
        ${QRC_FILES}
    )
else()
  add_executable(${APP_NAME}
        ${PROJECT_SOURCES}
    )

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(${APP_NAME}
  INTERFACE bcdice_irc_grpc_proto
  INTERFACE ${_REFLECTION}
  INTERFACE ${_GRPC_GRPCPP}
  INTERFACE ${_PROTOBUF_LIBPROTOBUF}
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
)
