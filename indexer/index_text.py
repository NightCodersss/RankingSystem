import sys
import nltk
import nltk.probability

if len(sys.argv) < 4:
	raise StandartError("usage: script.py <text_file> <doc_id> <commit_file>")

text_file = open(sys.argv[1])
doc_id = sys.argv[2]
commit = open(sys.argv[3], "a")

text = text_file.read().decode('utf-8') 

fdict = nltk.probability.FreqDist(token.lower().encode('utf-8') for token in nltk.word_tokenize(text))
for word in fdict:
	print "Word to commit: ", word
	commit.write("{word} {doc_id} {freq}\n".format(
		word = word, 
		doc_id = doc_id, 
		freq = fdict.freq(word)))
		
