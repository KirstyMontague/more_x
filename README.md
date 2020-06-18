# tests

This repository holds a preliminary attempt at building a system which evolves a simple robotic behaviour using DEAP and ARGoS.

The chromosomes are evaluated according to how far up the x axis a robot travels in 400 timesteps. Each solution is initialised with 10 robots in random starting positions, and each run is performed several times with different random seeds (these numbers can be adjusted to alter the accuracy of the averages and the effects of inter-robot interference).

The solutions are decision trees composed of the following nodes:

* If facing up the y axis
* If facing up the x axis
* Turn left
* Turn right
* Go forwards

Some brief experimentation has produced reasonable solutions with a population of less than 20 and within 20 generations.
