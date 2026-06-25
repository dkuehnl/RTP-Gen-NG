add_library(core
        StreamOptions.h
        StreamOptionValidator.cpp
        StreamOptionValidator.h
        RandomGenerators.cpp
        RandomGenerators.h
        YamlParser.cpp
        YamlParser.h
        ScenarioEngine.cpp
        ScenarioEngine.h
        PacketBuilder.cpp
        PacketBuilder.h
        #more .cpp-files placed here
)

include(FetchContent)

FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG yaml-cpp-0.9.0 # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
)
FetchContent_MakeAvailable(yaml-cpp)

target_link_libraries(core PUBLIC yaml-cpp::yaml-cpp) # The library or executable that require yaml-cpp library

target_include_directories(core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(core
    PUBLIC cxx_std_20
)