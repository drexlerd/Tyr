# List of subdirectory names
set(SUBDIRECTORIES "analysis" "buffer" "common" "formalism" "grounder" "planning" "solver")
# set(SUBDIRECTORIES "analysis" "buffer" "common" "formalism" "grounder" "planning" "solver")

# Initialize empty lists for source and header files
set(TYR_SRC_FILES "")
set(TYR_PRIVATE_HEADER_FILES "")
set(TYR_PUBLIC_HEADER_FILES "")

# Loop over the subdirectories for source files
foreach(subdir ${SUBDIRECTORIES})
    file(GLOB_RECURSE TMP_SRC_FILES "${subdir}/*.cpp" "${subdir}/**/*.cpp")
    list(APPEND TYR_SRC_FILES ${TMP_SRC_FILES})

    file(GLOB_RECURSE TMP_PRIVATE_HEADER_FILES "${subdir}/*.hpp" "${subdir}/**/*.hpp")
    list(APPEND TYR_PRIVATE_HEADER_FILES ${TMP_PRIVATE_HEADER_FILES})

    file(GLOB_RECURSE TMP_PUBLIC_HEADER_FILES 
        "../include/tyr/${subdir}/*.hpp" "../include/tyr/${subdir}/**/*.hpp")
    list(APPEND TYR_PUBLIC_HEADER_FILES ${TMP_PUBLIC_HEADER_FILES})
endforeach()

list(APPEND TYR_PUBLIC_HEADER_FILES "../include/tyr/tyr.hpp")

add_library(core STATIC ${TYR_SRC_FILES} ${TYR_PRIVATE_HEADER_FILES} ${TYR_PUBLIC_HEADER_FILES})
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
