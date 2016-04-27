import sys
import nltk
import nltk.probability
import nltk.stem
import nltk.corpus
import chardet

if len(sys.argv) < 4:
	raise StandartError("usage: script.py <text_id> <doc_id> <commit_file>")

text_id = sys.argv[1]
text_file = open(text_id + ".text")
doc_id = sys.argv[2]
commit = open(sys.argv[3], "a")

raw = text_file.read();
encoding = chardet.detect(raw)["encoding"]

## It was problems with screen
#if encoding not in ["utf-8", "ascii"]:
#	print "Unsupprted encoding: ", encoding
#	sys.exit(1)

text = raw.decode(encoding) 

stemmer = nltk.stem.SnowballStemmer("russian")

fdict = nltk.probability.FreqDist(stemmer.stem(token.lower()).encode('utf-8') 
	for token in nltk.word_tokenize(text) 
		if not token in nltk.corpus.stopwords.words("russian") 
			and len(token) >= 2)
for word in fdict:
	commit.write("{word} {text_id} {doc_id} {freq}\n".format(
		word = word, 
		text_id = text_id,
		doc_id = doc_id, 
		freq = fdict.freq(word)))
		
