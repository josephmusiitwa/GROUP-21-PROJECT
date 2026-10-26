# Example 1
add_executable(example1 example1.cpp)
target_link_libraries(example1 PRIVATE meanshift)

# Example 2 
add_executable(example2 example2.cpp)
target_link_libraries(example2 PRIVATE meanshift)

# Example 3
add_executable(example3 example3.cpp)
target_link_libraries(example3 PRIVATE meanshift)