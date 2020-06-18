add_library(footbot_deap SHARED footbot_deap.h footbot_deap.cpp node.h node.cpp)
target_link_libraries(footbot_deap
  argos3core_simulator
  argos3plugin_simulator_footbot
  argos3plugin_simulator_genericrobot)
