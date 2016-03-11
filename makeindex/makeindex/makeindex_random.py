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

def generate_index(number_of_docs, r, word):

	n = len(cs)

	inverse_index_files = [open('{word} {text_id} .index'.format(word=word, text_id=i), 'w') for i in range(n)]
	checkfile = open('checkfile', 'w')

	forward_index_list = open('forwardindexlist', 'a') # indexlist is generated in ../util/sortindex.py

	for doc_id in range(number_of_docs):
		forward_index_list.write("{} {} .forward\n".format(doc_id, word))
		with open("{} {} .forward".format(doc_id, word), "w") as forward_file:
			lambdas = generate_lambda(cs, r(doc_id + 1)) # monotonic 

			for text_id, (f, l) in enumerate(zip(inverse_index_files, lambdas)):
				f.write("{doc_id} {correspondence}\n".format(doc_id=doc_id, correspondence=l))
				forward_file.write("{text_id} {correspondence}\n".format(text_id=text_id, correspondence=l))

				checkfile.write("{doc_id} {rank}\n".format(doc_id=doc_id, rank=r(doc_id + 1)))
		
	forward_index_list.close()
	checkfile.close()

	map(file.close, inverse_index_files)
#	for f in files:
#		f.close()
	
	
def read_cs(filename):
	return map(float, open(filename, 'r').readline().split(' '))

cs = read_cs("makeindex_random.config")

def gen_rank(d):
	return d**(-1.2)
#	return d**-0.5

words_list = ["Fairytale", "Happiness"]

for word in words_list:
	generate_index(10000, gen_rank, word)
