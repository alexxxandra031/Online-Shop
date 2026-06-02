cmake_minimum_required(VERSION 3.16)
project(client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set(PROJECT_SOURCES
    main.cpp
)

add_executable(client ${PROJECT_SOURCES}
    clientmanager.h clientmanager.cpp
    mainwindow.h mainwindow.cpp
    authwindow.h authwindow.cpp
    adminwindow.h adminwindow.cpp
    managerwindow.h managerwindow.cpp
    clientwindow.h clientwindow.cpp
    mainwindow.ui
    authwindow.ui
    adminwindow.ui
    managerwindow.ui
    clientwindow.ui
)
target_link_libraries(client PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)
