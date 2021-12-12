import matplotlib.pyplot as plt
from operator import methodcaller
from operator import itemgetter
import sys

if len(sys.argv) < 3:
	print("Pass input and output file name as command line argument")
	exit(-1)

inputFilename = str(sys.argv[1])
outputFilename = str(sys.argv[2])
file = open(inputFilename, "r") 
lines = file.readlines()

nodes = int(lines[0])

gen_num = []
gen_gnome = []
for i in range(nodes):
	gen_gnome.append([])

for line in lines[1:-nodes-2]:
	num_node = int(line.split()[0].split("-")[0])
	if num_node == 0:
		gen = int(line.split()[0].split("-")[1])
		gen_num.append(gen)
	gen_fitness = float(line.split()[1])
	gen_gnome[num_node].append(gen_fitness)


time_sol = lines[-nodes-2:]
time_sol.sort()

solutions = time_sol[:2]
opt_sol = float(solutions[0].split()[1])
subopt_sol = float(solutions[1].split()[1])

times = list(map(int,map(itemgetter(1),map(methodcaller("split", "          "),time_sol[2:]))))

color = ['green', 'red', 'blue', 'black']

legendList = []
# plotting the points
for index, gen_node in enumerate(gen_gnome):
	plt.plot(gen_num, gen_node, color=color[index], linewidth = 1, marker='.', markerfacecolor=color[index], markersize=2)
	exectime = times[index]/1000000
	legendList.append("node " + str(index) + f": {exectime:.2f} s" )

plt.plot(gen_num[-1], opt_sol, color='yellow',marker='*', markersize=10, markeredgecolor='black')
legendList.append("opt. sol.:"+ str(opt_sol))

plt.plot(gen_num[-1], subopt_sol, color='pink',marker='X', markersize=10, markeredgecolor='black')
legendList.append("found sol.:"+ str(subopt_sol))

plt.legend(legendList)

# naming the x axis
plt.ylabel('Fitness')
# naming the y axis
plt.xlabel('Generation number')

# function to show the plot
# plt.show()
plt.savefig('../plots/' + outputFilename)
 