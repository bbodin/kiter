cmake_minimum_required(VERSION 3.14)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets Core Gui REQUIRED)

set(SOURCE_FILES
        qiter.cpp
        mainwindow.cpp
        mainwindow.ui
        graphwidget.cpp
        qtNode.cpp
        qtEdge.cpp
)

add_executable(qiter ${SOURCE_FILES})
target_link_libraries(qiter PRIVATE Qt5::Widgets ${KITER_LIBRARIES} -lz)
target_include_directories(qiter PRIVATE ${KITER_INCLUDE_DIR})

install(TARGETS qiter DESTINATION bin)
