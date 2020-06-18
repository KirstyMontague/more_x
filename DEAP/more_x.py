import copy
import random

import numpy

from functools import partial

from deap import algorithms
from deap import base
from deap import creator
from deap import tools
from deap import gp

import subprocess
import time
from operator import attrgetter


numRobots = 10
iterations = 3
	
	
def playbackBest(population):
	
	# get best
	for individual in population:		
		thisFitness = calculateFitness(individual.fitness.getValues())
		if ('best' not in locals() or thisFitness > bestFitness):
			best = individual
			bestFitness = thisFitness	
	
	# save
	with open('../best.txt', 'w') as f:
		print >> f, numRobots	
		print >> f, best	
		
	# run argos
	subprocess.call(["/bin/bash", "playback", "", "./"])


def calculateFitness(values):
	# apply a depth penalty to the raw fitness value
	penalty = values[1] - 2
	if penalty < 0:
		penalty = 0
	penalty /= 10
	return values[0] - penalty
	

def selTournament(individuals, k, tournsize, fit_attr="fitness"):
	
	# reproducing selTournament here because I can't find a way to get
	# algorithms.selTournament to take more than one fitness value
	# into account
	
	chosen = []
	for i in xrange(k):
		aspirants = tools.selRandom(individuals, tournsize)
		
		# get DEAP's best	
		best = max(aspirants, key=attrgetter(fit_attr))
		bestFitness = calculateFitness(best.fitness.getValues())
		
		# check for individuals with a better fitness once depth penalty has been applied
		for individual in aspirants:
			thisFitness = calculateFitness(individual.fitness.getValues())
			if (thisFitness > bestFitness):
				best = individual
				bestFitness = thisFitness
		
		chosen.append(best)
	return chosen


def varAnd(population, toolbox, cxpb, mutpb):
	
	# copied from the algorithms module with minimal adaptation
	
	offspring = [toolbox.clone(ind) for ind in population]

	# Apply crossover and mutation on the offspring
	for i in range(1, len(offspring), 2):
		if random.random() < cxpb:
			offspring[i - 1], offspring[i] = toolbox.mate(offspring[i - 1],
																		 offspring[i])
			del offspring[i - 1].fitness.values, offspring[i].fitness.values

	for i in range(len(offspring)):
		if random.random() < mutpb:
			offspring[i], = toolbox.mutate(offspring[i])
			del offspring[i].fitness.values

	return offspring


def eaTest(population, toolbox, cxpb, mutpb, ngen, stats=None, halloffame=None, verbose=__debug__):

	# copied from the algorithms module eaSimple with minimal modification

	logbook = tools.Logbook()
	logbook.header = ['gen', 'nevals'] + (stats.fields if stats else [])
	logbook.chapters["fitness"].header = "min", "avg", "max"
	logbook.chapters["size"].header = "min", "avg", "max"

	# Evaluate the individuals with an invalid fitness
	invalid_ind = [ind for ind in population if not ind.fitness.valid]
	fitnesses = toolbox.map(toolbox.evaluate, invalid_ind)
	for ind, fit in zip(invalid_ind, fitnesses):
		ind.fitness.values = fit

	if halloffame is not None:
		halloffame.update(population)

	record = stats.compile(population) if stats else {}
	logbook.record(gen=0, nevals=len(invalid_ind), **record)
	if verbose:
		print ""
		print logbook.stream
		print "-----------------------------------------------------------------------------------"

	# Begin the generational process
	for gen in range(1, ngen + 1):

		time.sleep(2)
	
		offspring = toolbox.select(population, len(population))
		
		# Vary the pool of individuals
		offspring = varAnd(offspring, toolbox, cxpb, mutpb)

		# Evaluate the individuals with an invalid fitness
		invalid_ind = [ind for ind in offspring if not ind.fitness.valid]
		fitnesses = toolbox.map(toolbox.evaluate, invalid_ind)
		for ind, fit in zip(invalid_ind, fitnesses):
			ind.fitness.values = fit

		# Update the hall of fame with the generated individuals
		if halloffame is not None:
			halloffame.update(offspring)

		# Replace the current population by the offspring
		population[:] = offspring

		# Append the current generation statistics to the logbook
		record = stats.compile(population) if stats else {}
		logbook.record(gen=gen, nevals=len(invalid_ind), **record)
		if verbose:
			print "-----------------------------------------------------------------------------------"
			print logbook.stream
			print "-----------------------------------------------------------------------------------"

	playbackBest(toolbox.select(population, len(population)))
	return population, logbook


def evaluateRobot(individual):
	
	with open('../chromosome.txt', 'w') as f:
		print >> f, numRobots	
		print >> f, individual		
	
	robots = {}
	for r in range(numRobots):
		robots[r] = {}

	seed = 0	
	runningFitness = 0
	for i in range(iterations):
		
		# write seed to file
		seed = seed + 1
		with open('../seed.txt', 'w') as f:
			print >> f, seed
		
		# run argos
		subprocess.call(["/bin/bash", "evaluate", "", "./"])
			
		# result from file	
		f = open("../result.txt", "r")
		
		# check each line for values that need saved
		for line in f:
			first = line[0:1]
			second = line[1:2]			
			if ((first == "s" or first == "f") and (second == "x" or second == "y" or second == "z" or second == "w")):
				line = line[3:]
				robotId = int(float(line[0:line.find(' ')]))
				line = line[line.find(' ')+1:-1]
				robots[robotId][first+second] = line
		
		# measure distance travelled up the x axis by each robot and add to cumulative total
		thisFitness = 0;
		for j in range(numRobots):
			distance = float(robots[j]["fx"]) - float(robots[j]["sx"])
			thisFitness += distance
		
		# divide to get average for this iteration and add to running total
		thisFitness /= numRobots
		runningFitness += thisFitness;
	
	# divide to get average per iteration
	fitness = runningFitness / iterations
	
	# print tree depth, fitness and chromosome
	print str(individual.height) + " " + str("%.6f" % fitness) + "\t" + str(individual)	
	
	time.sleep(.2)
	
	return (fitness, individual.height, )


class robotObject(object):

	def if_up(one, two): 
		print "if_up "
                
	def if_left(one, two): 
		print "if_left "
        
	def left(): 
		print "left "

	def right():
		print "right "
        
	def straight():
		print "straight "


robot = robotObject()

pset = gp.PrimitiveSet("MAIN", 0)
pset.addPrimitive(robot.if_up, 2)
pset.addPrimitive(robot.if_left, 2)
pset.addTerminal(robot.straight)
pset.addTerminal(robot.left)
pset.addTerminal(robot.right)

creator.create("FitnessMulti", base.Fitness, weights=(0.01,-2.0,))
creator.create("Individual", gp.PrimitiveTree, fitness=creator.FitnessMulti)

toolbox = base.Toolbox()

# Attribute generator
toolbox.register("expr_init", gp.genFull, pset=pset, min_=1, max_=2)

# Structure initializers
toolbox.register("individual", tools.initIterate, creator.Individual, toolbox.expr_init)
toolbox.register("population", tools.initRepeat, list, toolbox.individual)


toolbox.register("evaluate", evaluateRobot)
toolbox.register("select", selTournament, tournsize=4)
toolbox.register("mate", gp.cxOnePoint)
toolbox.register("expr_mut", gp.genFull, min_=0, max_=2)
toolbox.register("mutate", gp.mutUniform, expr=toolbox.expr_mut, pset=pset)





def main():
	random.seed(68)

	pop = toolbox.population(n=10)
	hof = tools.HallOfFame(1)
	
	stats_fit = tools.Statistics(key=lambda ind: ind.fitness.values[0])
	stats_size = tools.Statistics(key=lambda depth: depth.height)
	mstats = tools.MultiStatistics(fitness=stats_fit, size=stats_size)

	stats = tools.Statistics(lambda ind: ind.fitness.values)
	mstats.register("avg", numpy.mean)
	mstats.register("std", numpy.std)
	mstats.register("min", numpy.min)
	mstats.register("max", numpy.max)

	eaTest(pop, toolbox, 0.5, 0.2, 5, mstats, halloffame=hof)
    
	return pop, hof, mstats

if __name__ == "__main__":
	main()
