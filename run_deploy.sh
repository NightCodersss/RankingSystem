
if [ -z ${1+x} ]
then
	echo "./new_deploy <deploy_name>"
	exit 1
fi
deploy_name=$1
deploy_path="./deploys/$deploy_name"

cd $deploy_path
rm -f pids
./export_config.sh
nohup ./indexserver & echo $! >> pids
nohup ./forwardindexserver & echo $! >> pids
nohup ./forcerankingserver texts_config & echo $! >> pids
sleep 1
nohup ./rankingserver texts_config & echo $! >> pids
sleep 1
nohup ./southserver & echo $! >> pids
sleep 1
nohup python stem_server.py & echo $! >> pids
echo "Servers are done"




