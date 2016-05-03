import nltk
import nltk.stem

stemmer = nltk.stem.SnowballStemmer("russian")

def stem(word):
	return stemmer.stem(word.lower())
