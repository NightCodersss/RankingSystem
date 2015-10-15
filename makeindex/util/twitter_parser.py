# -*- encoding: utf-8 -*-
import json
import string
import sys
import transliteration
from collections import Counter 

search_words = [u"Обама", u"я", u"дом", u"Украина", u"Донецк", u"коридор"]

def get_json(filename):
	index = {}
#	index.setdefault(Counter([]))
	i = 0
	with open(filename) as f:
		for line in f:
#			print "--"
			try:
#			if True:
				i += 1
#				if i % 1000 == 0:
#					print i
				if len(line) < 5:
					continue
	#			line = line.decode("unicode-escape")
#				print line
#				print [ord(c) for c in line]
				
				j = json.loads(line)
#				print j["text"]
				text = j["text"]
#				print text.split()
				words = filter(lambda x: x in search_words, map(lambda x: filter(lambda x: x not in string.punctuation, x), text.split()))
#				for word in words:
#					print word
				hashtags = [d["text"].encode("utf-8") for d in j['entities']['hashtags']]
#				print "Hashtags:", hashtags
				for word in words:
					if word not in index:
						index[word] = Counter()
					index[word] += Counter(hashtags)
#					print hashtags
			except:
				pass
#				print "Pizoh", i
	return index
index = get_json(sys.argv[1])
f = open(sys.argv[2], 'w')
#print "########################"
#print ""
#print ""
#print index
#print "olo"
for word in index:
	for hashtag in index[word]:
		f.write(word.encode("utf-8"));
		f.write(" ")
		f.write(hashtag);
		f.write(" ")
		f.write(str(index[word][hashtag]).encode("utf-8"));
		f.write("\n")
