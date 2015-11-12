import subprocess
import sys
import time

def run_test(n):
#	subprocess.call("bash -c python ./ddos_script.py 1 test_{}.log > /dev/null".format(n).split(" "))
	subprocess.Popen(["bash", "-c", "python ./ddos_script.py 1 test_{}.log > /dev/null".format(n)])

def stat(n):
	t_min = 1e100
	t_max = 0
	t_avg = 0
	errors = 0
	c = 0
	for i in xrange(n):
		with open("test_{}.log".format(i)) as f:
			for t_str in f:
				t = float(t_str)

				if t == -1:
					errors += 1
					continue
				t_min = min(t, t_min)
				t_max = max(t, t_max)
				t_avg += t
				c += 1
	t_avg /= c
	return (t_min, t_avg, t_max, errors, errors / (c + errors))

if len(sys.argv) < 4:
	print "Usage: batch_size number_of_tests sleep_time" 
	sys.exit()

batch_size = int(sys.argv[1])
test_count = int(sys.argv[2])
sleep_time = float(sys.argv[3])

i = 0
for s in xrange(test_count):
	print "Session ", s, " started"
	for _ in xrange(batch_size):
		run_test(i)
		i += 1
	print "Session ", s, " ended, waiting"
	time.sleep(sleep_time)

print stat(i)
