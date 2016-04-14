
CURRENT_INDEX_PATH=../current_index/

index_pressa:
	echo "OLOLO!!! This has not been implemented yet"

index_random:
	(cd makeindex; ./makeindex_random.sh)
	mv makeindex/*.index $(CURRENT_INDEX_PATH)
	mv makeindex/*.forward $(CURRENT_INDEX_PATH)
	mv makeindex/checkfile $(CURRENT_INDEX_PATH)
	mv makeindex/indexlist $(CURRENT_INDEX_PATH)
	mv makeindex/forwardindexlist $(CURRENT_INDEX_PATH)
	echo "!!!!! checkfile does not seem to be correct"

index_random_2:
	(cd makeindex; ./makeindex_random_2.py)
	mv makeindex/*.index $(CURRENT_INDEX_PATH)
	mv makeindex/*.forward $(CURRENT_INDEX_PATH)
	mv makeindex/indexlist $(CURRENT_INDEX_PATH)
	mv makeindex/forwardindexlist $(CURRENT_INDEX_PATH)
	mv makeindex/*.txt $(CURRENT_INDEX_PATH)


index_istinausers:
	(cd makeindex; ./makeindex_istinausers.sh)
	mv makeindex/*.index $(CURRENT_INDEX_PATH)
	mv makeindex/indexlist $(CURRENT_INDEX_PATH)
	mv makeindex/forwardindexlist $(CURRENT_INDEX_PATH)
	# TODO: make backup with timestamp
	# TODO WARN: build forward index
	

index_twitter:
	(cd makeindex; ./makeindex_twitter.sh)
	mv makeindex/*.index $(CURRENT_INDEX_PATH)
	mv makeindex/indexlist $(CURRENT_INDEX_PATH)
	mv makeindex/forwardindexlist $(CURRENT_INDEX_PATH)
	# TODO WARN: build forward index

clean:
	rm -f $(CURRENT_INDEX_PATH)/*
