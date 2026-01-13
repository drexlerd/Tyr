# List of subdirectory names
set(SUBDIRECTORIES "analysis" "buffer" "common" "formalism" "datalog" "planning")
# set(SUBDIRECTORIES "analysis" "buffer" "common" "formalism" "datalog" "planning")

# Initialize empty lists for source and header files
set(TYR_SRC_FILES "")
set(TYR_PRIVATE_HEADER_FILES "")
set(TYR_PUBLIC_HEADER_FILES "")

# Loop over the subdirectories for source files
foreach(subdir ${SUBDIRECTORIES})
    file(GLOB_RECURSE TMP_PRIVATE_HEADER_FILES "${subdir}/*.hpp" "${subdir}/**/*.hpp")
    list(APPEND TYR_PRIVATE_HEADER_FILES ${TMP_PRIVATE_HEADER_FILES})

    file(GLOB_RECURSE TMP_PUBLIC_HEADER_FILES 
        "../include/tyr/${subdir}/*.hpp" "../include/tyr/${subdir}/**/*.hpp")
    list(APPEND TYR_PUBLIC_HEADER_FILES ${TMP_PUBLIC_HEADER_FILES})
endforeach()

list(APPEND TYR_PUBLIC_HEADER_FILES "../include/tyr/tyr.hpp")

add_library(core STATIC ${TYR_PRIVATE_HEADER_FILES} ${TYR_PUBLIC_HEADER_FILES}
    analysis/listeners.cpp
    analysis/program_domains.cpp
    analysis/stratification.cpp
    analysis/task_domains.cpp

    datalog/workspaces/d2p.cpp
    datalog/workspaces/facts.cpp
    datalog/workspaces/program.cpp
    datalog/workspaces/rule_delta.cpp
    datalog/workspaces/rule.cpp
    datalog/workspaces/worker.cpp
    datalog/bottom_up.cpp
    datalog/consistency_graph.cpp
    datalog/delta_kpkc.cpp
    datalog/fact_set.cpp
    datalog/formatter.cpp
    datalog/rule_scheduler.cpp

    planning/ground_task/axiom_evaluator.cpp
    planning/ground_task/axiom_stratification.cpp
    planning/ground_task/node.cpp
    planning/ground_task/state_repository.cpp
    planning/ground_task/state.cpp
    planning/ground_task/successor_generator.cpp

    planning/lifted_task/axiom_evaluator.cpp
    planning/lifted_task/node.cpp
    planning/lifted_task/state_repository.cpp
    planning/lifted_task/state.cpp
    planning/lifted_task/successor_generator.cpp
    planning/lifted_task/task_grounder.cpp

    planning/programs/action.cpp
    planning/programs/axiom.cpp
    planning/programs/common.cpp
    planning/programs/rpg.cpp
    planning/programs/ground.cpp

    planning/algorithms/gbfs_lazy.cpp
    planning/algorithms/gbfs_lazy/event_handler.cpp

    planning/action_executor.cpp
    planning/domain.cpp
    planning/formatter.cpp
    planning/ground_task.cpp
    planning/lifted_task.cpp
    planning/loki_to_tyr.cpp
    planning/parser.cpp
    planning/task_utils.cpp
)
set_target_properties(core PROPERTIES OUTPUT_NAME tyr_core)

# Create an alias for simpler reference
add_library(tyr::core ALIAS core)

target_link_libraries(core
  PUBLIC
    Threads::Threads
    TBB::tbb
    valla::core   
    loki::parsers  
  PRIVATE
    fmt::fmt
)

# Use include depending on building or using from installed location
target_include_directories(core
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

# Install the target and create export-set
install(
    TARGETS core
    EXPORT tyrcoreTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Generate and install export file
install(EXPORT tyrcoreTargets
    NAMESPACE tyr::
    COMPONENT core
    FILE "tyrcoreTargets.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/tyr"
)

# Generate build tree export file
export(EXPORT tyrcoreTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/tyrcoreTargets.cmake"
    NAMESPACE tyr::
)
