# GUMMY QUBO Solver

A heuristic solver for Quadrantic Unconstrained Binary Optimization (QUBO) Problems.

Group 08:
1. Michael Adipoetra - 408045 
2. Gennesaret Kharistio Tjusila - 407687 
3. Francisco Jose Manjon Cabeza Garcia - 369293 
4. Arya Prasetya - 473184

# Run using HPC
Before running the code, you can change the number of iteration in "create_jobs.py" on line 10. Default number of iteration is 100.

```
python3 create_jobs.py
bash run_job.sh
```


# How to build Qubo in your own machine

First, compile the source code while creating a build directory, i.e. in root. 

```
bash compile.sh
```

Then you can run the program. The code takes 3 inputs:
1. Path to the instance from QPLIB
2. Path to the solution of this instance from QPLIB
3. Name of the solver

Use the following command to run the QUBO solver by changing {instances_name} and {solver_name}. Optionally {timelimit}

```
./build/gummy ./problem_instances/qplib/instances/QPLIB_{instances_name}.qs ./problem_instances/qplibsolution/QPLIB_{instances_name}_answer.qs {solver_name} {timelimit(seconds):optional}
```

we have the following instances and solver:

instance_name = ['3506', '3565', '3642', '3650', '3693', '3705', '3706', '3738', '3745', '3822', '3832', '3838', '3850', '3852', '3877', '5721', '5725', '5755', '5875', '5881', '5882', '5909', '5922']

solver_name = ['DTSS', 'MSTS', 'SA']

For example:
```
./build/gummy ./problem_instances/qplib/instances/QPLIB_3506.qs ./problem_instances/qplib/solution/QPLIB_3506_answer.qs DTSS 5 
```
The above solves QPLIB_3506.qs with the DTSS solver for 5 second.

DTSS = Diversification Driven Tabu Search

MSTS = Multistart Tabu Search

SA = Simulated Annealing