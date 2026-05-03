if (NOT ENABLE_TESTS)
    return()
endif()

set(TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")

include(FetchContent)

# --- Google Test ---
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# Windows-spezifisch: erzwingt den gleichen CRT wie Parent
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# verhindert, dass GoogleTest die Toolchain überschreibt
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
FetchContent_MakeAvailable(googletest)

# --- Test Executable ---
add_executable(core_tests
        StreamOptionValidatorBasicTest.cpp
        StreamOptionValidatorScenarioTest.cpp
)

target_compile_definitions(core_tests
        PRIVATE
        TEST_DATA_DIR="${TEST_DATA_DIR}"
)

target_link_libraries(core_tests
        PRIVATE
        core
        gtest_main
        gmock
)

include(GoogleTest)
gtest_discover_tests(core_tests)
