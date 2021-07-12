import os
from sys import argv

#compilacion = "gcc -o agentes_test agentes.cpp -lstdc++ -O3 -march=native -fopenmp"
compiler   = "gcc -o "
target     = "agentes_test "
program    = "agentes.cpp "
flags      = "-lstdc++ -pedantic-errors " # -Wall " #-Werrors
opt_flags  = "-O3 -march=native -fopenmp "

compilacion =compiler + target + program + flags + opt_flags
print(compilacion)

flush = "del "+"agentes_test.exe"

os.system("%s" %compilacion)
try:
	flusher = int(argv[1])
	if flusher:
		print("Flush")
		os.system("%s" %flush)
except:
	pass




