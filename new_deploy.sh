if [ -z ${1+x} ]
then
	echo "./new_deploy <deploy_name>"
	exit 1
fi
deploy_name=$1
deploy_path="./deploys/$deploy_name"

rm -rf $deploy_path
mkdir -p $deploy_path
make
cp index_server/indexserver $deploy_path 
cp forward_index_server/forwardindexserver $deploy_path 
cp force_ranking_server/forcerankingserver $deploy_path
cp ranking_server/rankingserver $deploy_path
cp ranking_server/config_file $deploy_path/texts_config
cp south_protocol/southserver $deploy_path
cp stem_server/stem_server.py $deploy_path
cp export_config.sh $deploy_path
echo "change config in $deploy_path/export_config.sh and $deploy_path/texts_config"
# WARN do not copies libraries

cp storage/bigtest $deploy_path
