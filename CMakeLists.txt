cmake_minimum_required(VERSION 3.16)
project(m4s_d VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

if(WIN32)
    qt_add_executable(m4s_d WIN32 main.cpp)
else()
    qt_add_executable(m4s_d main.cpp)
endif()

target_link_libraries(m4s_d PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)

set_target_properties(m4s_d PROPERTIES
    OUTPUT_NAME "m4s_d"
    WIN32_EXECUTABLE TRUE
)

if(UNIX AND NOT APPLE)
    include(GNUInstallDirs)
    install(TARGETS m4s_d DESTINATION ${CMAKE_INSTALL_BINDIR})
    configure_file(
        "${CMAKE_SOURCE_DIR}/m4s_d.desktop.in"
        "${CMAKE_BINARY_DIR}/m4s_d.desktop"
        @ONLY
    )
    install(
        FILES "${CMAKE_BINARY_DIR}/m4s_d.desktop"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/applications"
    )
endif()
