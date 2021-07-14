import matplotlib.pyplot as plt
import numpy as np 
import math
rho = 1000./math.pow(150,2)
print("Densidad: %f" %rho)
alpha = math.sqrt(1./rho)

N = np.array([1000., 10000.,50000.,100000.])
print("#agents    :", N)
L = alpha*np.sqrt(N)
print("System size:",L)

system_memory = 40*N/1_000_000.
size_memory   = L*L*48/1_000_000


print(system_memory)
print(size_memory)
#plt.semilogx(N,L)
#plt.plot(4*N/1000000.,L*L*48/1000000.)
#plt.show() 