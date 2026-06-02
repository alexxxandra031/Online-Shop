cmake_minimum_required(VERSION 3.16)
project(server VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network)

set(PROJECT_SOURCES
    main.cpp
)

add_executable(server ${PROJECT_SOURCES}
    tcpserver.h tcpserver.cpp
    clienthandler.h clienthandler.cpp
    databasemanager.h databasemanager.cpp
    models.h models.cpp

)
target_link_libraries(server PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)
