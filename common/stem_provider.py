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

def get_hash(word):
	return hash(word) % 2**62

saved_hashes = set([])
word_hash_file = open(os.environ["RANKING_SYSTEM_HOME"]+"/indexer/words.hash_list", "a")
def reg_hash(word):
	hash_ = get_hash(word)
	if hash_ not in saved_hashes:
		saved_hashes.add(hash_)
		word_hash_file.write(str(hash_))
		word_hash_file.write(" ")
		word_hash_file.write(word.encode('utf-8'))
		word_hash_file.write("\n")
	return str(hash_)

def stem(word):
	if stem_policity == "nltk":
		return reg_hash(stemmer.stem(word.lower()))
	elif stem_policity == "pymorphy":
		common = nltk.probability.FreqDist(form.normal_form for form in morph.parse(word.lower())).most_common(1)
		return reg_hash(common[0][0])
