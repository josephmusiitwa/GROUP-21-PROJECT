# Example 1
add_executable(example example.cpp)
target_link_libraries(example2 PRIVATE meanshift)

# Example 2 (External Data)
add_executable(example2 example2.cpp)
target_link_libraries(example2 PRIVATE meanshift)