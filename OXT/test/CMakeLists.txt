file(GLOB_RECURSE VHCSSE_TEST_SOURCES CONFIGURE_DEPENDS ./*.cpp)
set(CMAKE_CXX_STANDARD 17)

foreach(test_source ${VHCSSE_TEST_SOURCES})
    # Create a human readable name.
    get_filename_component(test_filename ${test_source} NAME)
    string(REPLACE ".cpp" "" test_name ${test_filename})

    # Add the test target separately and as part of "make check-tests".
    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} gtest gtest_main crypto mysqlclient crypto_lib 
    filter_lib pbc gmp)
    set_target_properties(${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/test)
endforeach()