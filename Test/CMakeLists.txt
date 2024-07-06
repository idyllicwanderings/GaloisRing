
enable_testing()

add_executable(test_example tests/test_example.cpp)

add_test(NAME ExampleTest COMMAND test_example)