if [ -z ${1+x} ]
then
	echo "./new_index <index_name>"
	exit 1
fi
index_name=$1
mkdir -p "./indexes/$index_name/index/"
mkdir -p "./indexes/$index_name/forward/"
echo "Set FORWARD_INDEX_PATH and INDEX_PATH to ./indexes/$index_name/index ./indexes/$index_name/forward"
