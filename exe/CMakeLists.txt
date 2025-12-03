add_executable(ground_task "ground_task.cpp")
target_link_libraries(ground_task tyr::core)

add_executable(lifted_task "lifted_task.cpp")
target_link_libraries(lifted_task tyr::core)
