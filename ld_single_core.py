import networkx as nx
import egttools as egt
import numpy as np
import random
import matplotlib.pyplot as plt

def printProgressBar (iteration, total, prefix = '', suffix = '', decimals = 1, length = 100, fill = '█', printEnd = "\r"):
  percent = ("{0:." + str(decimals) + "f}").format(100 * (iteration / float(total)))
  filledLength = int(length * iteration // total)
  bar = fill * filledLength + '-' * (length - filledLength)
  print(f'\r{prefix} |{bar}| {percent}% {suffix}', end = printEnd)
  # Print New Line on Complete
  if iteration == total:
    print()

def do_I_replace(px, py, kx, ky, T, S):
  if px < py:
    kg = kx if kx>ky else ky
    a = T if T>1 else 1
    b = S if S<0 else 0
    Dg = a-b
    prob = (py - px)/(kg*Dg)
    if np.random.rand() < prob:
      return True
  return False

@profile
def run_execution_on_graph(graph, N, n_generations, payoff_matrix, multimode=False):
  population = [0, 1] * (N//2)
  for _ in range(5):
    np.random.shuffle(population)

  full_sums = np.zeros(n_generations+1)

  next_population = population.copy()
  payoffs = np.zeros(N)

  for i in range(n_generations + 1):
    if not multimode:
      printProgressBar(i, n_generations, prefix = 'Progress:', suffix = 'Complete', length = 50)

    for j in range(N):
      payoffs[j] = 0
      for neighbour in graph[j].keys():
        payoffs[j] += payoff_matrix[population[j], population[neighbour]]

    for k in range(N):
      neighbours = list(graph[k].keys())
      random_neighbour_id, random_neighbour_strat = random.choice(list(enumerate(neighbours)))
      px, py = payoffs[k], payoffs[random_neighbour_id]
      kx, ky = len(neighbours), len(graph[random_neighbour_id].keys())

      T = payoff_matrix[0,1]
      S = payoff_matrix[1,0]

      if do_I_replace(px,py,kx,ky,T,S):
        next_population[k] = population[random_neighbour_strat]

    population[:] = next_population[:]
    full_sums[i] = np.sum(population)
  return full_sums

def exec_with_fixed_params(N, z, n_realizations, n_runs, n_generations, n_transient, S, T, R=1, P=0, multimode=False, tid=0):
  payoff_matrix = np.array([[P,T],[S,R]])
  sums_real_av = np.zeros(n_realizations)
  for realization in range(n_realizations):
    graph = nx.barabasi_albert_graph(N,z)
    sums_av = np.zeros(n_runs)
    for run in range(n_runs):
      if not multimode:
        print("\nRealization {:d}/{:d}, run {:d}/{:d}".format(realization+1,n_realizations,run+1,n_runs))
        print("------------------------------")
        print("Transient period:")
      else:
        print("[Thread {}] {}{}/{}".format(tid, realization, run+1, n_realizations*n_runs))
      _ = run_execution_on_graph(graph, N, n_transient, payoff_matrix, multimode)
      
      if not multimode:
        print("Evaluation period:")
      sums = run_execution_on_graph(graph, N, n_generations, payoff_matrix, multimode)
      sums_av[run] = np.mean(sums)
    sums_real_av[realization] = np.mean(sums_av)
  return np.mean(sums_real_av)


N = 100 # Size of population
z = 4 # Average connectivity
granularity = 8
n_transient = 100
n_generations = 100
n_realizations = 2 # Number of realizations, where we change the graph 
n_runs = 2 # Number of runs for the same realization (same graph)

results = open("results_single.txt", "a")

tid = "t1"
tid_int = int(tid[1])
n_threads = 8

R = 1 
P = 0
Ss = [-1]

Ts = np.linspace((2/n_threads)*(tid_int-1),(2/n_threads)*(tid_int),granularity//n_threads)
    
j = 1
for S in Ss:
  i = 1
  for T in Ts:
    print("[NEWEVAL][Thread {}] Evaluating with new T ({}/{}) -- S: ({}/{})".format(tid,i,len(Ts),j,len(Ss)))
    retval = exec_with_fixed_params(N, z, n_realizations, n_runs, n_generations, n_transient, S, T, multimode=True, tid=tid)
    print(T)
    print(S)
    print(retval)
    results.write("{}, {}, {}\n".format(T,S,retval))
    results.write(str(S))
    results.write(str(retval))
    i+=1
  j+=1



