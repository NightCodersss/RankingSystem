#!/usr/bin/python

import socket, sys, os, threading, re, time

RESULT_SIZE = 10
REFRESH_TIME = 0.5 

hist = [0] * 100
size_errors = [0] # hack to make varibale shared among threads

def getnum(s):
	idx = s.find("docid")
	if idx != -1:
		n = -1
		str_to_parse = s.split()[-1]
		print "str_to_parse:", str_to_parse
		try:
			n = int(str_to_parse)
		except Exception as e:
			print e
			print s
			return -1
		finally:
			return n
	else:
		return -1

def permutation_metric(a):
	return sum([abs(i - gi) for i, gi in enumerate(a)])


def attack():
	result = []
	try:
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
		s.connect(("localhost", 15000))
		print "connected"
		s.send("Fairytale\n")
		print "sent"
		time.sleep(1)
		full_answer = ""
		while True:
			st = s.recv(4096)
			time.sleep(0.5)


			full_answer += st

			if st == '':
				break
			else:
#				print st
				pass

		idxs = filter(lambda x: x != -1, map(getnum, full_answer.split('\n')))
		result.extend(idxs)

		s.close()  
	except Exception as e:
		print e

	if len(result) != RESULT_SIZE:
#		print "Result size is not correct:", len(result)
		size_errors[0] += 1
	else:
		hist[permutation_metric(result)] += 1


def show_status():
	print "Errors:", size_errors[0]
	success = sum(hist)
	print "Success: ", success 
	print "Hist:", hist
	if success + size_errors[0] < N:
                threading.Timer(REFRESH_TIME, show_status).start()
	    

threading.Timer(0, show_status).start()

N = int(sys.argv[1])

for i in range(N):
	threading.Timer(3 * i, attack).start()

while True:
    success = sum(hist)
    if success + size_errors[0] >= N:
        break

