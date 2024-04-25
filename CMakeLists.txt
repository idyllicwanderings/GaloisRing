cmake_minimum_required(VERSION 3.10)

project(galoisring)

# For debugging
add_definitions("-Wall -g")

# Set C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories("include")


# Testing
enable_testing()
file(GLOB TEST_SOURCES "test/*.c")

# Iterate over the list and create an executable for each file
foreach(TEST_SOURCE ${TEST_SOURCES})
  # Get the file name without extension
  get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

  # Create the executable
  add_executable(${TEST_NAME} ${TEST_SOURCE})

  # Link the necessary libraries
  target_link_libraries(${TEST_NAME}
    # TODO: lib to link
    GTest::gtest_main
  )

  # Add the test to Google Test
  gtest_discover_tests(${TEST_NAME})
endforeach()




