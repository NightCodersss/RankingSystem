import subprocess
from random import gauss, uniform

def generate_lambda(c, r): # d is id of document (number)
	l2 = sum(map(lambda x: x**2, c))
	a = min(r, 1 - r) / (2 * l2 ** 0.5)
	y = [uniform(-a, a) for _ in c]
	x = map(lambda y, x: y - x + r, y, map(lambda x: x * sum(map(lambda a, b: a * b, y, c)) / l2, c))
	return x

def generate_file(number_of_docs, r):

	n = len(cs)

	files = [open('index_{}.dat'.format(i), 'w') for i in range(n)]
	checkfile = open('checkfile', 'w')

	for d in range(number_of_docs):		
		lambdas = generate_lambda(cs, r(d+1))
#		(lambdas, r) = generate_lambda(K)
		map(lambda (f, l): f.write("Fairytale {} {}\n".format(d, l)), zip(files, lambdas))
		checkfile.write("{} {}\n".format(d, r(d+1)))
	
	checkfile.close()
	map(lambda f: f.close(), files)
	
	
def read_cs(filename):
	return map(float, open(filename, 'r').readline().split(' '))

cs = read_cs("generating_config")

def gen_rank(d):
	#return d**(-1.2)
	return d**-0.5

generate_file(10000, gen_rank)
