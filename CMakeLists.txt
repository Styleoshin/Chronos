cmake_minimum_required(VERSION 2.8)

project(Chronos)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find the QtWidgets library
find_package(Qt5 REQUIRED COMPONENTS Widgets WebEngineWidgets Network Sql)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})
include_directories(${Qt5WebEngineWidgets_INCLUDE_DIRS})
include_directories(${Qt5Sql_INCLUDE_DIRS})

include_directories(src)
add_subdirectory(src)

#Ressouce icons
set(RESOURCE icons/ressources.qrc)
qt5_add_resources(RESOURCE_ADDED ${RESOURCE})

#Icons app
set (ICON_APP icons/app_icon.rc)

set (MAIN src/main.cpp)

add_executable(${PROJECT_NAME} ${MAIN} ${RESOURCE_ADDED} ${ICON_APP})

target_link_libraries(${PROJECT_NAME} source  Qt5::Widgets Qt5::Sql Qt5::Network Qt5::WebEngineWidgets)


