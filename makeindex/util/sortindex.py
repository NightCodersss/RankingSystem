import os
from os import listdir
from os.path import isfile, join

with open('indexlist', 'w') as indexlist:
	for f in listdir("./"): 
		if isfile(join("./",f)) and f.endswith('.index'):
			with open(f, 'r') as index, open("sorted_" + f, 'w') as sorted_index:
				sorted_index.write(''.join(sorted(index.readlines(), key=lambda s: float(s.split(' ')[1]), reverse=True)))
			os.rename("sorted_" + f, f)
			indexlist.write(f + '\n')
