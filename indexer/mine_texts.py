import sys

# creates files for text of document in current folder
# output: files: <text_id>.text

if len(sys.argv) < 2:
	raise StandartError("usage: script.py <doc to parse>")

# TODO: make text separation
doc_name = sys.argv[1]
with open(doc_name) as doc, open("full.text", "w") as text:
	text.write(doc.read())

