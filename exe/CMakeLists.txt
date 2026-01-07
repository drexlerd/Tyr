add_executable(brfs "brfs.cpp")
target_link_libraries(brfs tyr::core)

add_executable(gbfs_lazy "gbfs_lazy.cpp")
target_link_libraries(gbfs_lazy tyr::core)

add_executable(ground_task "ground_task.cpp")
target_link_libraries(ground_task tyr::core)

add_executable(lifted_task "lifted_task.cpp")
target_link_libraries(lifted_task tyr::core)
