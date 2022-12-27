import networkx as nx
import egttools as egt
import numpy as np
import random
import matplotlib.pyplot as plt

#def printProgressBar (iteration, total, prefix = '', suffix = '', decimals = 1, length = 100, fill = '█', printEnd = "\r"):
#  percent = ("{0:." + str(decimals) + "f}").format(100 * (iteration / float(total)))
#  filledLength = int(length * iteration // total)
#  bar = fill * filledLength + '-' * (length - filledLength)
#  print(f'\r{prefix} |{bar}| {percent}% {suffix}', end=printEnd)
#  # Print New Line on Complete
#  if iteration == total:
#    print()

def do_I_replace(px, py, kx, ky, T, S):
  if px < py:
    kg = kx if kx > ky else ky
    maxT = T if T>1 else 1
    minS = S if S<0 else 0
    Dg = T-S
    prob = (py - px)/(kg*Dg)
    if np.random.rand() < prob:
      return True 
  return False

@profile
def run_execution_on_graph(graph, graph_dict_array, N, n_generations, payoff_matrix, multimode=False):

  T = payoff_matrix[0,1]
  S = payoff_matrix[1,0]
  R = payoff_matrix[1,1]
  P = payoff_matrix[0,0]

  population = np.zeros(N,dtype=int)
  ones = np.random.permutation(N)[:N//2]
  for el in ones:
    population[el] = 1

  full_sums = np.zeros(n_generations+1)

  next_population = population.copy()
  payoffs = np.zeros(N)

  for i in range(n_generations + 1):
    if not multimode:
      #printProgressBar(i, n_generations, prefix = 'Progress:', suffix = 'Complete', length = 50)
      pass

    for j in range(N):
      payoffs[j] = 0
      for l in graph_dict_array[j]:
        payoffs[j] += payoff_matrix[population[j], population[l]]

    for k in range(N):
      neighbours = graph_dict_array[k]
      random_neighbour_id = int(len(neighbours)*random.random())
      random_neighbour_strat = neighbours[random_neighbour_id]
      px, py = payoffs[k], payoffs[random_neighbour_id]
      kx, ky = len(neighbours), len(graph_dict_array[random_neighbour_strat])

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

    # Transforming graph #
    graph_dict_array = {}
    for key in graph:
      graph_dict_array[key] = np.zeros(len(graph[key].keys()),dtype=int)
      el = 0
      for neig in graph[key].keys():
        graph_dict_array[key][el] = neig
        el +=1
    # ------------------ #
    sums_av = np.zeros(n_runs)
    for run in range(n_runs):
      if not multimode:
        print("\nRealization {:d}/{:d}, run {:d}/{:d}".format(realization+1,n_realizations,run+1,n_runs))
        print("------------------------------")
        print("Transient period:")
      else:
        print("[Thread {}] {}{}/{}".format(tid, realization, run+1, n_realizations*n_runs))
      _ = run_execution_on_graph(graph, graph_dict_array, N, n_transient, payoff_matrix, multimode)
      
      if not multimode:
        print("Evaluation period:")
      sums = run_execution_on_graph(graph, graph_dict_array, N, n_generations, payoff_matrix, multimode)
      sums_av[run] = np.mean(sums)
    sums_real_av[realization] = np.mean(sums_av)
  return np.mean(sums_real_av)