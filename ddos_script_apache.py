import socket, sys, os, threading, re, time

RESULT_SIZE = 10
REFRESH_TIME = 0.5 

hist = [0] * 100
size_errors = [0] # hack to make varibale shared among threads

def getnum(s):
	idx = s.find("docid")
	if idx != -1:
		n = -1
		try:
			n = int(s[idx + 9: -1])
		except Exception as e:
			print e
			print s
			return -1
		finally:
			return n
	else:
		return -1

def permutation_metric(a):
	return reduce(lambda x, y: x + y, [abs(i - gi) for i, gi in enumerate(a)])


def attack():

	full_answer = ""

	try:
		#pid = os.fork()  
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
		s.connect(("localhost", 80))
		s.send("GET /\n");
#		print "OLOLOTROLOLO"
#		s.settimeout(10)
		time.sleep(1)
		while True:
			st = s.recv(4096)


			full_answer += st

			if st == '':
				break
			else:
#				print st
				pass

#		idxs = filter(lambda x: x != -1, map(getnum, full_answer.split('\n')))
#		result.extend(idxs)

		s.close()  
	except Exception as e:
		print e

#	if len(result) != RESULT_SIZE:
#		print "Result size is not correct:", len(result)
#		size_errors[0] += 1
#	else:
#		hist[permutation_metric(result)] += 1

	if full_answer.strip(' \t\r\n') == "5000050000":
		hist[0] += 1
	else:
		size_errors[0] += 1
	

N = int(sys.argv[1])
for i in range(N):
	threading.Timer(0, attack).start()

while True:
	time.sleep(REFRESH_TIME)
	print "Errors:", size_errors[0]
	success = sum(hist)
	print "Success: ", success 
	print "Hist:", hist
	if success + size_errors[0] == N:
		break


