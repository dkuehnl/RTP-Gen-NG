add_executable(cli_app
    main.cpp
        yaml_template.h
        CliParser.cpp
        CliParser.h
        TemplateEditor.cpp
        TemplateEditor.h
        RtpEngine.cpp
        RtpEngine.h
)

target_include_directories(cli_app PRIVATE ${CMAKE_SOURCE_DIR}/external)

target_link_libraries(cli_app
    PRIVATE core
)