import subprocess
from random import gauss, uniform

def generate_lambda(c, r): # d is id of document (number)
	"""
		See documentation in developer documentation 
	"""
	l2 = sum(map(lambda x: x**2, c))
	a = min(r, 1 - r) / (3 * len(c) ** 0.5 * l2 ** 0.5) # len(c)**0.5 is for diagonal of n-dimensional cube. We want it to be inside of vector c.
	y = [uniform(-a, a) for _ in c]
	x = map(lambda y, x: y - x + r, 
			y, 
			map(lambda x: x * sum(map(lambda a, b: a * b, y, c)) / l2, c))
	return x

def generate_file(number_of_docs, r):

	n = len(cs)

	files = [open('index.{}.index'.format(i), 'w') for i in range(n)]
	checkfile = open('checkfile', 'w')

	for d in range(number_of_docs):	# d is document id
		lambdas = generate_lambda(cs, r(d + 1)) # monotonal 

		for f, l in zip(files, lambdas):
			f.write("Fairytale {} {}\n".format(d, l))

		checkfile.write("{} {}\n".format(d, r(d+1)))
	
	checkfile.close()

	for f in files:
		f.close()
	
	
def read_cs(filename):
	return map(float, open(filename, 'r').readline().split(' '))

cs = read_cs("makeindex_random.config")

def gen_rank(d):
	return d**(-1.2)
#	return d**-0.5

generate_file(10000, gen_rank)
