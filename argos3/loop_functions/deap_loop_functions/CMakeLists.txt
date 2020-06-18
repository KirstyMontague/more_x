link_directories(${CMAKE_BINARY_DIR}/controllers/footbot_deap)

set(deap_loop_functions_SOURCES deap_loop_functions.h deap_loop_functions.cpp)

add_library(deap_loop_functions MODULE  ${deap_loop_functions_SOURCES})

target_link_libraries(deap_loop_functions
                       footbot_deap
							  argos3core_simulator
							  argos3plugin_simulator_entities
							  argos3plugin_simulator_footbot)

