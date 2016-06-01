
if [ -z ${1+x} ]
then
	echo "./index_one_document.sh <deploy_name> <abs_path_to_file>"
	exit 1
fi
if [ -z ${2+x} ]
then
	echo "./index_one_document.sh <deploy_name> <abs_path_to_file>"
	exit 1
fi
deploy_name=$1
file=$2
deploy_path="$RANKING_SYSTEM_HOME/deploys/$deploy_name"

cd $deploy_path
. ./export_config.sh
printenv
rm -f commit hashes
echo "$file" | python $RANKING_SYSTEM_HOME/indexer/index.py $deploy_path/commit $RANKING_SYSTEM_HOME/indexer/hashes # hashes are common for all indexes (it's like rainbow tables)

# forward
mv *.forward $FORWARD_INDEX_PATH

# inversed
cat commit | ./bigtest
cd -


