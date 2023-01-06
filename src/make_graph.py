import networkx as nx
import numpy as np

N = 1000
z = 4
n_realizations = 10

graph = nx.complete_graph(N)
#graph = nx.barabasi_albert_graph(N,z)

for i in range(n_realizations):
  with open("/Users/mos/Documents/ULB/MA3.tmp/INFO-F409/Exam/LDProject/graphs/py_graph_{}.txt".format(i),"w") as file:
    for key in graph:
      i = 0
      for el in graph[key]:
        if i == 0:
          file.write("{}".format(el))
          i+=1
        else:
          file.write(",{}".format(el))
      file.write("\n")
