add_subdirectory(core)

if (ENABLE_CLI)
    add_subdirectory(cli)
endif()

if (ENABLE_GUI)
    add_subdirectory(gui)
endif()
