export PRESSA_HOME=/home/asio/pressa/mnt/backup2/serg/pressa

# usage: "make doc DOC=/data/doc1.txt"
doc:
	# DOC is varible for path to document
	./index.sh $(DOC)

all:
	./index_pressa.sh

clean:
	rm -f commit hashes *.pyc words.hash_list *.forward
