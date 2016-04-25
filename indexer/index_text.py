import sys
import nltk

if sys.argc < 3:
	raise StandartError("usage: script.py <text_file> <commit_file>")

text_file = open(sys.argv[1])
commit = open(sys.argv[2], "a")

text = "" # TODO read all text from file

freq = dict()

for token in nltk.word_tokenize(text):

