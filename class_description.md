# Class Description

## Model Class
A "Model" class represents an optimization problem instance. It's main duty is to keep information about the instance
and the last solve process (found solution and runtime). It must be able to:
- Save The matrix Q which defines our problem
- have a solve method which calls the solve method of the solver the instance
- A solver variable which contains a solver object that is user to solve the instance
- The possibility to get the returned value of the last solve
- The possibility to get the run time of the last solve

## Solver Abstract Class
A "Solver" is a class whose input is a matrix Q, solving meta parameter such as time limit and ram limit with 1 method solve
It must be able to:
- have a solve method which returns the best found solution and the runtime needed to find a solution

## SingleRunSolver Class
The single run solver class is an instantiation of the abstract solver class for single run solve. It does a single solve and return the answer directly.

## MultiRunSolver Class
The multi run solver class is an instantiation of the abstract solver class for multiple run solve. It does multiple solve and return the best found solution and the total run time. It has a method `getAllSolution` that returns all solution found.
 
## Initializer Abstract Class
An initializer accepts as input a length n and generate an initial solution of length n. How this initial solution is generated is defined in each instantiation of the class

## TabuSearch Abstract Class
A TabuSearch accepts as input the current solution and returns as output a set of candidates to evaluate. How this is done is defined in each instantiation of the class. It will only be initiated once at the start of the solving process. Thus, it may also have some form of memory.

## Evaluate Abstract Class
An evaluate abstract class gets as input a set of candidate solution and determine a candidate (one or some small amount) to investigate in the next step.

## Solve Sketch

1. Call Initializer to initiate an answer. set current answer.
2. Use Tabu search to determine a set of candidates based on current answer 
3. Use evaluate to determine which candidate to run next. 
4. Change current answer. If termination condition is reached, go to 5. Otherwise back to 2
5. Return answer

