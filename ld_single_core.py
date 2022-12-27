from ld_func import *


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
    results.write("{}, {}, {}\n".format(T,S,retval))
    i+=1
  j+=1
results.close()



