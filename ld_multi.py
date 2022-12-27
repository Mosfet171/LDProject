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
  if px > py:
    return False
  else: 
    kg = np.max((kx,ky))
    Dg = np.max((T,1)) - np.min((S,0))
    prob = (py - px)/(kg*Dg)
    if np.random.rand() < prob:
      return True 
    else:
      return False

#@profile
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
        if neighbour > j: # In order not to count twice because we added the payoff of the neighbour directly ...
          payoffs[j] += payoff_matrix[population[j], population[neighbour]]
          payoffs[neighbour] += payoff_matrix[population[neighbour], population[j]] # .... here.

    for k in range(N):
      neighbours = list(graph[k].keys())
      random_neighbour_id, random_neighbour_strat = random.choice(list(enumerate(neighbours)))
      px, py = payoffs[k], payoffs[random_neighbour_id]
      kx, ky = len(neighbours), len(graph[random_neighbour_id].keys())

      T = payoff_matrix[0,1]
      S = payoff_matrix[1,0]

      ## REPLACING DO I REPLACE
      if px < py:
        #kg = np.max((kx,ky))
        kg = kx if kx>ky else ky
        #Dg = np.max((T,1)) - np.min((S,0))
        a = T if T>1 else 1
        b = S if S<0 else 0
        Dg = a-b
        prob = (py - px)/(kg*Dg)
        if np.random.rand() < prob:
          next_population[k] = population[random_neighbour_strat]
      ######################
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

import threading

def exec_for_thread(results):
  N = 100 # Size of population
  z = 4 # Average connectivity
  granularity = 8
  n_transient = 100
  n_generations = 100
  n_realizations = 2 # Number of realizations, where we change the graph 
  n_runs = 2 # Number of runs for the same realization (same graph)
  
  tid = threading.current_thread().name
  tid_int = int(tid[1])
  n_threads = 8
  
  R = 1 
  P = 0
  Ss = np.linspace(-1,1,2)
  
  Ts = np.linspace((2/n_threads)*(tid_int-1),(2/n_threads)*(tid_int),granularity//n_threads)
      
  i = 1
  for S in Ss:
    for T in Ts:
      print("\n[----***-----]\n[NEWEVAL][Thread {}] Evaluating with new T ({}/{})\n[----***-----]\n".format(tid,i,len(Ts)))
      retval = exec_with_fixed_params(N, z, n_realizations, n_runs, n_generations, n_transient, S, T, multimode=True, tid=tid)
      print(T)
      print(S)
      print(retval)
      results.write("{}, {}, {}\n".format(T,S,retval))
      results.write(str(S))
      results.write(str(retval))
      i+=1

results_t1 = open("results_t1.txt", "a")
results_t2 = open("results_t2.txt", "a")
results_t3 = open("results_t3.txt", "a")
results_t4 = open("results_t4.txt", "a")
results_t5 = open("results_t5.txt", "a")
results_t6 = open("results_t6.txt", "a")
results_t7 = open("results_t7.txt", "a")
results_t8 = open("results_t8.txt", "a")
t1 = threading.Thread(target=exec_for_thread, args=(results_t1,), name="t1")
t2 = threading.Thread(target=exec_for_thread, args=(results_t2,), name="t2")
t3 = threading.Thread(target=exec_for_thread, args=(results_t3,), name="t3")
t4 = threading.Thread(target=exec_for_thread, args=(results_t4,), name="t4")
t5 = threading.Thread(target=exec_for_thread, args=(results_t5,), name="t5")
t6 = threading.Thread(target=exec_for_thread, args=(results_t6,), name="t6")
t7 = threading.Thread(target=exec_for_thread, args=(results_t7,), name="t7")
t8 = threading.Thread(target=exec_for_thread, args=(results_t8,), name="t8")


t1.start()
t2.start()
t3.start()
t4.start()
t5.start()
t6.start()
t7.start()
t8.start()

t1.join()
t2.join()
t3.join()
t4.join()
t5.join()
t6.join()
t7.join()
t8.join()

print("Done!")

