#!/usr/bin/python
import sys
import nltk
import nltk.probability
import nltk.corpus
import chardet
import stem_provider
import hashlib

# stdin: pipe of doc's pathes

if len(sys.argv) < 3:
	raise StandartError("usage: script.py <commit_file> <hashes>")

commit_file = open(sys.argv[1], "a")
hashes_file = open(sys.argv[2], "a")

def save_hash(doc_path):
	key = hash(doc_path) % 2**64
	hashes_file.write("{} {}\n".format(key, doc_path));
	return key

def load_doc(doc_path):
	with open(doc_path) as doc_file:
		raw = doc_file.read()
		encoding = chardet.detect(raw)["encoding"]
		return raw.decode(encoding)

# Textes : array<{text_id: string, text: string}>
def get_texts(doc):
	sentences = doc.split('.')
	return [
		{
			"text_id": "first_sentence",
			"text": sentences[0]
		},
		{
			"text_id": "body",
			"text": '.'.join(sentences[1:-1])
		}
	]

n = 0;
for doc_path in sys.stdin:
	try:
		print "n: ", n
		n += 1
		doc_path = doc_path.rstrip()
		print "doc: ", doc_path
		doc_id = save_hash(doc_path)
		print "doc_id: ", doc_id
		doc = load_doc(doc_path);
		texts = get_texts(doc);
		for text in texts:
			fdict = nltk.probability.FreqDist(stem_provider.stem(token).encode('utf-8') 
				for token in nltk.word_tokenize(text["text"]) 
					if not token in nltk.corpus.stopwords.words("russian") 
						and len(token) >= 2)
			for word in fdict:
				commit_file.write("{word} {text_id} {doc_id} {freq}\n".format(
					word = word, 
					text_id = text["text_id"],
					doc_id = doc_id, 
					freq = fdict.freq(word)))
			
	except Exception, e:
		print "Error: ", e
