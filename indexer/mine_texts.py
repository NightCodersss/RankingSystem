import sys

# creates files for text of document in current folder
# output: files: <text_id>.text

if len(sys.argv) < 2:
	raise StandartError("usage: script.py <doc to parse>")

doc_name = sys.argv[1]

texts = set([
	#"full", 
	"first_sentence", 
	"body"
	])

if "full" in texts:
	with open(doc_name) as doc, open("full.text", "w") as text:
		text.write(doc.read())
elif "first_sentence" in texts:
	with open(doc_name) as doc, \
		open("first_sentence.text", "w") as first_sentence_file, \
		open("body.text", "w") as body_file:

		sentences = doc.read().split('.')

		first_sentence_file.write(sentences[0])
		body_file.write('.'.join(sentences[1:-1]))
