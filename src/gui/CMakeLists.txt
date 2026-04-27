find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_ATUORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(gui_app
    main.cpp
    #mainwindow.cpp
    #mainwindow.h
)

target_link_libraries(gui_app
    PRIVATE
        core
        Qt6::Widgets
)