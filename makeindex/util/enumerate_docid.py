
#This script replaces hashtags with numbers

filename = "index_2.dat"
f = open(filename, 'r')

newfilename = "index_2.datnew"
newf = open(newfilename, 'w')

max_hash = 10**10

for line in f:
	w, h, c = line.split()
	h = hash(h) % max_hash
	newf.write("{} {} {}\n".format(w, h, c))

f.close()
newf.close()
