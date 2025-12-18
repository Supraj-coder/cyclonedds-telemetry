add_executable(sensor_hub main.cpp)
target_include_directories(sensor_hub PRIVATE ${CMAKE_SOURCE_DIR}/src/core)
target_link_libraries(sensor_hub PRIVATE core dds_io)
