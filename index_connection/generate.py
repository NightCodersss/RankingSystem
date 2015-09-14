import subprocess
from random import gauss, uniform

def generate_lambda(K): # d is id of document (number)
	n = len(cs)
	lambdas = [min(1., max(0, gauss(0.5, K))) for i in range(n)]
	r = reduce(lambda acc, (c, l): acc + c * l, zip(cs, lambdas), 0)
	return (lambdas, r)

def generate_file(number_of_docs, K):

	n = len(cs)

	files = [open('index_{}.dat'.format(i), 'w') for i in range(n)]
	checkfile = open('checkfile', 'w')

	for d in range(number_of_docs):
		(lambdas, r) = generate_lambda(K)
		map(lambda (f, l): f.write("Fairytale {} {}\n".format(d, l)), zip(files, lambdas))
		checkfile.write("{} {}\n".format(d, r))
	
	checkfile.close()
	map(lambda f: f.close(), files)
	
	
def read_cs(filename):
	return map(float, open(filename, 'r').readline().split(' '))

cs = read_cs("generating_config")

generate_file(10000, 0.4)
