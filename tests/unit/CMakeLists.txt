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

add_gtest(common_bit_packed_layout                       "common/bit_packed_layout.cpp")

# add_gtest(analysis_domains                               "analysis/domains.cpp")
# add_gtest(analysis_stratification                        "analysis/stratification.cpp")
# add_gtest(analysis_listeners                             "analysis/listeners.cpp")

# add_gtest(buffer_indexed_hash_set                        "buffer/indexed_hash_set.cpp")

add_gtest(formalism_builder                              "formalism/builder.cpp")
add_gtest(formalism_repository                           "formalism/repository.cpp")
add_gtest(formalism_view                                 "formalism/view.cpp")

# add_gtest(grounder_assignment_sets                       "grounder/assignment_sets.cpp")
# add_gtest(grounder_generator                             "grounder/generator.cpp")
# add_gtest(grounder_static_consistency_graph              "grounder/static_consistency_graph.cpp")

# add_gtest(planning_lifted_task                           "planning/lifted_task.cpp")
# add_gtest(planning_ground_task                           "planning/ground_task.cpp")