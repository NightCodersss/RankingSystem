if [ -z "$COMMON_EXPORTS_ARE_DONE" ] 
then
	echo "Make common exports";
	exit 1;
fi
id=$1
FS=' '
grep $1 $RANKING_SYSTEM_HOME/indexer/hashes | awk '{print $2}' | xargs cat
