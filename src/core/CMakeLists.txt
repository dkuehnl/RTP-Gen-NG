add_library(core
        calculator.cpp
        #more .cpp-files placed here
)

target_include_directories(core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(core
    PUBLIC cxx_std_20
)