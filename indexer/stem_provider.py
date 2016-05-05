stem_policity = "pymorphy" # nltk, pymorphy

if stem_policity == "nltk":
	import nltk
	import nltk.stem
	stemmer = nltk.stem.SnowballStemmer("russian")

elif stem_policity == "pymorphy":
	import nltk.probability
	import pymorphy2
	morph = pymorphy2.MorphAnalyzer()

else:
	import sys
	print "unknown stem_policity: ", stem_policity
	sys.exit(1)

def stem(word):
	if stem_policity == "nltk":
		return stemmer.stem(word.lower())
	elif stem_policity == "pymorphy":
		common = nltk.probability.FreqDist(form.normal_form for form in morph.parse(word.lower())).most_common(1)
		return (common[0][0])
