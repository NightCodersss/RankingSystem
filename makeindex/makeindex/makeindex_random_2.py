#!/usr/bin/env python
import numpy as np
from random import gauss, uniform

# Generate index only for one word
# It's useful for testing operations

words = ["fairytale", "happiness"]
D = 100

model = np.array([[uniform(0, 1) ** 20 for _, i in enumerate(words)] for doc_id in range(D)]) # [doc_id, word_id]

#generating inverse
forward_index_list = open('forwardindexlist', 'w')
index_list = open('indexlist', 'w')
for word, docs in zip(words, model.T):
	inverse_index_file = open('{word} {text_id} .index'.format(word=word, text_id=0), 'w')

	index_list.write('{word} {text_id} .index\n'.format(word=word, text_id=0))

	for doc_id, rank in sorted(enumerate(docs), key=lambda (doc_id, rank): -rank):
		inverse_index_file.write("{doc_id} {rank}\n".format(doc_id=doc_id, rank=rank))

		forward_index_list.write("{} {} .forward\n".format(doc_id, word))

		with open("{} {} .forward".format(doc_id, word), "w") as forward_file:
			forward_file.write("{text_id} {rank}\n".format(text_id=0, rank=rank))
	inverse_index_file.close()

for i1, w1 in enumerate(words):
	for i2, w2 in enumerate(words):
		if w1 == w2:
			continue
		query = "{} !{}".format(w1, w2)
		with open(query + ".txt", "w") as f:
			data = sorted(enumerate((model.T[i1] - model.T[i2]).tolist()), key=lambda (doc_id, rank): -rank)
			for doc_id, rank in data:
				f.write("{doc_id} {rank}\n".format(doc_id = doc_id, rank = rank))


