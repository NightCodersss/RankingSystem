
CURRENT_INDEX_PATH=../current_index/

index_pressa:

index_random:
	(cd makeindex; ./makeindex_random.sh)
	mv makeindex/*.index $(CURRENT_INDEX_PATH)
	mv makeindex/checkfile $(CURRENT_INDEX_PATH)
	echo "!!!!! checkfile does not seem to be correct"


index_istinausers:
	(cd makeindex; ./makeindex_istinausers.sh)
	mv makeindex/*.index $(CURRENT_INDEX_PATH)
	

index_twitter:
	(cd makeindex; ./makeindex_twitter.sh)
	mv makeindex/*.index $(CURRENT_INDEX_PATH)

clean:
	rm -f $(CURRENT_INDEX_PATH)/*
