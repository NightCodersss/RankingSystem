id=$1
FS=' '
grep $1 $RANKING_SYSTEM_HOME/indexer/hashes | awk '{print $2}' | xargs cat
