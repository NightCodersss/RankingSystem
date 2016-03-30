#!/usr/bin/python

import socket, sys, os, threading, re, time

RESULT_SIZE = 10
REFRESH_TIME = 0.5 
ATTACK_INTERVAL = 1.


# Usage: ./ddos_script.py [number of asks] [check file for rmse test]
N = int(sys.argv[1])
checkfile = sys.argv[2]

hist = [0] * 100
size_errors = [0] # hack to make varibale shared among threads
rmse = []

def parse_data(s): # (doc_id, rank) rank = -1 if is absent
	if s.find("doc_id") != -1:
		n = -1
		str_to_parse = s.split()[-1]
#		print "str_to_parse:", str_to_parse
		try:
			n = int(str_to_parse)
		except Exception as e:
			print e
			print s
		finally:
			return (n, -1)
	elif s.find("rank") != -1:
		n = -1
		str_to_parse = s.split()[-1][0 : -1]
#		print "str_to_parse rank:", str_to_parse
		try:
			rank = float(str_to_parse)
		except Exception as e:
			print e
			print s
		finally:
			return (-1, rank)
	else:
		return (-1, -1)
	print "olo error"

def permutation_metric(a):
	return sum([abs(i - gi) for i, gi in enumerate(a)])


def attack():
	perm = []
	ranks = []
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

		(perm, ranks) = zip(*map(parse_data, full_answer.split('\n')))
		perm = filter(lambda x: x != -1, perm)
		ranks = filter(lambda x: x != -1, ranks)

		s.close()  
	except Exception as e:
		print e

	if len(perm) != RESULT_SIZE or len(ranks) != RESULT_SIZE:
		print "Result size is not correct:", len(perm)
		size_errors[0] += 1
	else:
		hist[permutation_metric(perm)] += 1
		s = 0
		with open(checkfile) as f:
			for r1, r2 in zip(f, ranks):
				s += (float(r1) - r2)**2

		rmse.append(s)


def show_status():
	print "Errors:", size_errors[0]
	success = sum(hist)
	print "Success: ", success 
	print "Hist: ", hist
	print "RMSE: ", rmse
	if success + size_errors[0] < N:
                threading.Timer(REFRESH_TIME, show_status).start()
	    

threading.Timer(0, show_status).start()

for i in range(N):
	threading.Timer( ATTACK_INTERVAL * i, attack).start()

while True:
    success = sum(hist)
    if success + size_errors[0] >= N:
        break

