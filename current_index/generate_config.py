from os import listdir
from os.path import isfile, join
import json
import re
import numpy

onlyfiles = [f for f in listdir("./") if isfile(join("./",f)) and f.startswith('sorted')]

cs = list(numpy.loadtxt("/home/asio/rankingsystem/makeindex/makeindex/makeindex_random.config"))

output = {
	"texts": []
}

for c, f in zip(cs, sorted(onlyfiles, key=lambda f: int(re.search("\d+", f).group(0)))):
	text = {
		"servers": [
			{
				"host": "localhost",
				"port": "16000",
			}
		],
		"index_id": re.search("\d+", f).group(0),
		"factor": c,
	}
	output["texts"].append(text)

print json.dumps(output, indent=4)
