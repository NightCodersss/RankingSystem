import subprocess
from random import gauss, uniform

def generate_lambda(r, K): # d is id of document (number)
	n = len(cs)
	lambdas = [max(0.1, gauss(0.5, K)) for i in range(n)]
	s = reduce(lambda acc, (c, l): acc + c * l, zip(cs, lambdas), 0)
	return map(lambda l: l * r/s, lambdas)

def generate_file(number_of_docs, K):

	n = len(cs)

	files = [open('index_{}.dat'.format(i), 'w') for i in range(n)]
	checkfile = open('checkfile', 'w')

	for d in range(number_of_docs):
		r = uniform(0, 100)
		lambdas = generate_lambda(r, K)
		map(lambda (f, l): f.write("Fairytale {} {}\n".format(d, l)), zip(files, lambdas))
		checkfile.write("{} {}\n".format(d, r))
	
	checkfile.close()
	map(lambda f: f.close(), files)
	
	
def read_cs(filename):
	return map(float, open(filename, 'r').readline().split(' '))

cs = read_cs("generating_config")

generate_file(100, 3)
