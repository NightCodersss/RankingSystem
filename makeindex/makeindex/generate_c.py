import random
import matplotlib.pyplot as plt

def generate(n, s):
	numbers = [random.random() for _ in xrange(n)]
	numbers += [0, s]
	numbers.sort()

	result = []
	for i in xrange(1, len(numbers)):
		result.append(numbers[i] - numbers[i - 1])

	result.sort()

#	plt.imshow(result, aspect='auto')
#	plt.show()

#	plt.hist(result, bins=100)
	
#	plt.plot(result)
#	plt.show()

	return result

print ' '.join(map(str, generate(3, 1.0)))
