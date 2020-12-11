import pygad
import numpy as np
import bdh_boucwen
import time

def read_data():
    read_file = np.genfromtxt('./selected/0.txt', delimiter=',')

    global tt, des_pos, des_vel, act_pos, dt
    tt = []
    des_pos = []
    des_vel = []
    act_pos = []

    for i in range(2000):  ## DATA={t(i), d_pos(i), d_vel(i), a_pos(i)},i=1..2000
        if i > 0:
            if read_file[i - 1, 0] != read_file[i, 0]:
                tt.append(read_file[i, 0])
                des_pos.append(read_file[i, 1])
                des_vel.append(read_file[i, 2])
                act_pos.append(read_file[i, 3])

    # print(len(tt), len(des_pos), len(des_vel), len(act_pos))
    dt = 0.001

read_data()
function_inputs = bdh_boucwen.init_Random_Population(1, 7)[0]#[4,-2,3.5,5,-11,-4.7] # Function inputs.
print("function_inputs", function_inputs)
print("initial cost= ", bdh_boucwen.CostFunction(function_inputs, des_vel, dt, des_pos, act_pos))

def fitness_func(solution, solution_idx):
    #print("solution",solution)
    fitness = 1.0/ (bdh_boucwen.CostFunction(solution, des_vel, dt, des_pos, act_pos)+0.000001)
    #fitness = bdh_boucwen.CostFunction(solution, des_vel, dt, des_pos, act_pos)
    return  fitness

fitness_function = fitness_func

num_generations = 100 # Number of generations.
num_parents_mating = 10 # Number of solutions to be selected as parents in the mating pool.

# To prepare the initial population, there are 2 ways:
# 1) Prepare it yourself and pass it to the initial_population parameter. This way is useful when the user wants to start the genetic algorithm with a custom initial population.
# 2) Assign valid integer values to the sol_per_pop and num_genes parameters. If the initial_population parameter exists, then the sol_per_pop and num_genes parameters are useless.
sol_per_pop = 20 # Number of solutions in the population.
num_genes = len(function_inputs)


# init_range_low = [-0.01, 0, -0.01, -1, 0, 0, -5]
# init_range_high = [0.05, 0.05, 0.01, 10, 5, 25, 5]
init_range_low = [-1, 0, -0.1, -5, 0, 0, -5]
init_range_high = [0.1, 0.1, 0.1, 10, 5, 25, 5]

parent_selection_type = "sss" # Type of parent selection.
keep_parents = -1 # Number of parents to keep in the next population. -1 means keep all parents and 0 means keep nothing.

crossover_type = "single_point" # Type of the crossover operator.

# Parameters of the mutation operation.
mutation_type = "random" # Type of the mutation operator.
mutation_percent_genes = 10 # Percentage of genes to mutate. This parameter has no action if the parameter mutation_num_genes exists or when mutation_type is None.

last_fitness = 0
def callback_generation(ga_instance):
    global last_fitness
    print("Generation = {generation}".format(generation=ga_instance.generations_completed))
    #print("Fitness    = {fitness}".format(fitness=ga_instance.best_solution()[1]))
    #print("Change     = {change}".format(change=ga_instance.best_solution()[1] - last_fitness))
    last_fitness = ga_instance.best_solution()[1]

# Creating an instance of the GA class inside the ga module. Some parameters are initialized within the constructor.
ga_instance = pygad.GA(num_generations=num_generations,
                       num_parents_mating=num_parents_mating,
                       fitness_func=fitness_function,
                       sol_per_pop=sol_per_pop,
                       num_genes=num_genes,
                       init_range_low=init_range_low,
                       init_range_high=init_range_high,
                       parent_selection_type=parent_selection_type,
                       keep_parents=keep_parents,
                       crossover_type=crossover_type,
                       mutation_type=mutation_type,
                       mutation_percent_genes=mutation_percent_genes,
                       on_generation=callback_generation)

# Running the GA to optimize the parameters of the function.
t_start = time.time()
ga_instance.run()
t_finish = time.time()

# After the generations complete, some plots are showed that summarize the how the outputs/fitenss values evolve over generations.
ga_instance.plot_result()

# Returning the details of the best solution.
solution, solution_fitness, solution_idx = ga_instance.best_solution()
print("Parameters of the best solution : {solution}".format(solution=solution))
print("Fitness value of the best solution = {solution_fitness}".format(solution_fitness=solution_fitness))
print("Index of the best solution : {solution_idx}".format(solution_idx=solution_idx))

prediction = np.sum(np.array(function_inputs)*solution)
print("Predicted output based on the best solution : {prediction}".format(prediction=prediction))

if ga_instance.best_solution_generation != -1:
    print("Best fitness value reached after {best_solution_generation} generations.".format(best_solution_generation=ga_instance.best_solution_generation))


print("solution ", solution)
print("final cost=", bdh_boucwen.CostFunction(solution, des_vel, dt, des_pos, act_pos))
print("processing time=",(t_finish-t_start))
# Saving the GA instance.
filename = 'genetic' # The filename to which the instance is saved. The name is without extension.
ga_instance.save(filename=filename)

# Loading the saved GA instance.
loaded_ga_instance = pygad.load(filename=filename)
loaded_ga_instance.plot_result()
