find_package(Python 3.8 REQUIRED
  COMPONENTS Interpreter Development.Module
  OPTIONAL_COMPONENTS Development.SABIModule)

find_package(nanobind CONFIG REQUIRED PATHS ${CMAKE_PREFIX_PATH})
  # Set result variables
find_package(nanobind)
if(nanobind_FOUND)
   message(STATUS "Found nanobind: ${NB_DIR} (found version ${nanobind_VERSION})")
endif()


nanobind_add_module(pytyr
    main.cpp
    formalism/bindings.cpp
    formalism/planning/bindings.cpp
    planning/bindings.cpp
)

target_link_libraries(pytyr PRIVATE tyr::core)

install(TARGETS pytyr DESTINATION "pytyr")

## Stubgen
set(MODULES
    # pytyr  # Handled by __init__.py
    pytyr
    pytyr.formalism
    pytyr.formalism.planning
    pytyr.planning
)

foreach(mod IN LISTS MODULES)
    string(REPLACE "." "_" mod_target "${mod}")
    set(mod_target "${mod_target}_stub")
    string(REPLACE "." "/" mod_path "${mod}")
    set(mod_output "${mod_path}/__init__.pyi")

    nanobind_add_stub(
        "${mod_target}"
        MODULE "${mod}"
        OUTPUT "${mod_output}"
        PYTHON_PATH "."
        DEPENDS pytyr
    )

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${mod_output}"
            DESTINATION ${mod_path})
endforeach()
