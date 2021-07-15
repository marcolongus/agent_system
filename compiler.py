import os
from sys import argv
from termcolor import colored,cprint
import os
os.system('color')

#compilacion = "gcc -o agentes_test agentes.cpp -lstdc++ -O3 -march=native -fopenmp"
compiler   = "gcc -o "
target     = "agentes_test "
program    = "agentes.cpp "
flags      = "-lstdc++ -lm " # -Wall " #-Werrors
opt_flags  = "-O3 -march=native -fopenmp "

compilacion =compiler + target + program + flags + opt_flags
cprint(compilacion, 'green')

flush = "rm "+"agentes_test"

os.system("%s" %compilacion)
try:
	flusher = int(argv[1])
	if flusher:
		print("Flush")
		os.system("%s" %flush)
except:
	pass




