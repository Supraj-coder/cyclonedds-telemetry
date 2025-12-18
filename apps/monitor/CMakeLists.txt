add_executable(monitor main.cpp)
target_include_directories(monitor PRIVATE ${CMAKE_SOURCE_DIR}/src/core)
target_link_libraries(monitor PRIVATE core dds_io)
