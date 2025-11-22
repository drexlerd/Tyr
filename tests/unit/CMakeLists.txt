enable_testing()

find_package(GTest "1.11.0" REQUIRED COMPONENTS GTest Main PATHS ${CMAKE_PREFIX_PATH} NO_DEFAULT_PATH)
# Set result variables
find_package(GTest)

# Helper function to create and register a test executable
function(add_gtest test_name source_file)
    add_executable(${test_name} ${source_file})
    target_link_libraries(${test_name} PRIVATE tyr::core GTest::GTest GTest::Main)
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

# Add each test source file as a separate test executable
# add_gtest(analysis_domains                              "analysis/domains.cpp")


# add_gtest(formalism_repository                          "formalism/repository.cpp")
# add_gtest(formalism_proxy                               "formalism/proxy.cpp")

add_gtest(analysis2_domains                               "analysis2/domains.cpp")

add_gtest(cista_indexed_hash_set                          "cista/indexed_hash_set.cpp")

add_gtest(formalism2_repository                           "formalism2/repository.cpp")
add_gtest(formalism2_proxy                                "formalism2/proxy.cpp")