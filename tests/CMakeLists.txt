if(NOT BUILD_TESTS)
  return()
endif()

add_executable(test_serialization test_serialization.cpp)
target_include_directories(test_serialization PRIVATE ${CMAKE_SOURCE_DIR}/src/core)
target_link_libraries(test_serialization PRIVATE core Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(test_serialization)
