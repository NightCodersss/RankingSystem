import socket, sys, os, threading, re, time

RESULT_SIZE = 10

hist = [0] * 100
size_errors = [0] # hack to make varibale shared among threads

def getnum(s):
	idx = s.find("docid")
	if idx != -1:
		return int(s[idx + 9: -1])
	else:
		return -1

def permutation_metric(a):
	return reduce(lambda x, y: x + y, [abs(i - gi) for i, gi in enumerate(a)])


def attack():

	result = []

	try:
		#pid = os.fork()  
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
		s.connect(("localhost", 15000))
		s.send("Fairytale\n");
#		print "OLOLOTROLOLO"
#		s.settimeout(10)
		time.sleep(1)
		while True:
			st = s.recv(4096)

			#a = re.search('"docid" : (\d+)', st).group(0)

			idxs = filter(lambda x: x != -1, map(getnum, st.split('\n')))
#			print idxs
			result.extend(idxs)

			if st == '':
				break
			else:
#				print st
				pass

		s.close()  
	except Exception as e:
		print e

	if len(result) != RESULT_SIZE:
#		print "Result size is not correct:", len(result)
		size_errors[0] += 1
	else:
		hist[permutation_metric(result)] += 1

for i in range(1, 1000):
#	attack()  
	threading.Timer(0, attack).start()

for i in range(10):
	time.sleep(1)
	print "Errors:", size_errors[0]
	print "Hist:", hist


