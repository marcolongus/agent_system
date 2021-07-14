import matplotlib.pyplot as plt
import numpy as np 
import math
from termcolor import colored,cprint
import os

os.system('color')


rho = 1000./math.pow(150,2)

print("Densidad: %f" %rho),print()
alpha = math.sqrt(1./rho)

N = np.array([1000., 10_000.,100_000., 1_000_000.])
print("#agents    :", N)
L = alpha*np.sqrt(N)
print("System size:",L)

system_memory_float  = 40*N/1_000_000.
system_memory_double = 2*40*N/1_000_000.
size_memory   = L*L*48/1_000_000.

plt.title("Aprox. Global use of memory")

plt.ylim(0,size_memory.max())
plt.xlim(0,system_memory_double.max())
plt.xlabel("System memory [Mb]")
plt.ylabel("Syze system memory [Mb]")

plt.plot(system_memory_float,size_memory, label ="float")
plt.plot(system_memory_double,size_memory, label="double")

plt.legend()
plt.show()

