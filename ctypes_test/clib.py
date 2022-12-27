import ctypes
import numpy
import glob
import networkx as nx

# find the shared library, the path depends on the platform and Python version
libfile = glob.glob('build/*/run_execution_on_graph*.so')[0]

# 1. open the shared library
mylib = ctypes.CDLL(libfile)

# 2. tell Python the argument and result types of function mysum
mylib.run_execution_on_graph.restype = ctypes.c_float
mylib.run_execution_on_graph.argtypes = [numpy.ctypeslib.ndpointer(dtype=numpy.int32), ctypes.c_int, ctypes.c_int, ctypes.c_float]

array = numpy.arange(0, 100000000, 1, numpy.int32)

# 3. call function mysum
graph = numpy.array([[1,2],[0,2],[0,1],[0,2]],dtype=numpy.int32)
N = 10
n_gen = 1 
payoff_matrix = numpy.array([[1,2],[3,4]])

testing = mylib.run_execution_on_graph(graph, N, n_gen, payoff_matrix)