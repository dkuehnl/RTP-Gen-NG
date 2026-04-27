add_executable(cli_app
    main.cpp
)

target_link_libraries(cli_app
    PRIVATE core
)