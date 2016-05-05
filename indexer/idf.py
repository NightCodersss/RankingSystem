import sys
import nltk
import nltk.probability
import nltk.corpus
import chardet
import math
import stem_provider

if len(sys.argv) < 2:
	raise StandartError("usage: script.py <idf_file>")

idf_file = open(sys.argv[1], "w")

fdict = nltk.probability.FreqDist() # contains number of document by word
D = 0 # number of documents

for doc_file_name in sys.stdin:
	try:
		doc_file_name = doc_file_name.rstrip()
		print "D: ", D
		print "Doc file name: ", doc_file_name
		if doc_file_name == "":
			break

		with open(doc_file_name) as doc_file:
			raw = doc_file.read();
		encoding = chardet.detect(raw)["encoding"]

		text = raw.decode(encoding)
		doc_dist = nltk.probability.FreqDist() 
		for stemed_token in [stem_provider.stem(token).encode('utf-8') for token in nltk.word_tokenize(text) if not token in nltk.corpus.stopwords.words("russian") and len(token) >= 2]:
			doc_dist[stemed_token] = 1;
		fdict += doc_dist
		D += 1
	except Exception, e:
		print "Error: ", e

for word in fdict:
	idf_file.write("{word} {idf} {docs}\n".format(
		word = word, 
		idf = math.log( D * 1. / fdict[word] ),
		docs = fdict[word]))
		
