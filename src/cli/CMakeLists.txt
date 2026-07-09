add_executable(cli_app
    main.cpp
        yaml_template.h
        CliParser.cpp
        CliParser.h
        TemplateEditor.cpp
        TemplateEditor.h
        RtpEngine.cpp
        RtpEngine.h
        IControlChannel.h
        UnixSocketControlChannel.cpp
        UnixSocketControlChannel.h
        AmiControlChannel.cpp
        AmiControlChannel.h
)

target_include_directories(cli_app PRIVATE ${CMAKE_SOURCE_DIR}/external)
set_target_properties(cli_app PROPERTIES OUTPUT_NAME rtpgen-ng)

target_link_libraries(cli_app
    PRIVATE core
)