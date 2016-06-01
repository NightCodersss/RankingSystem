
if [ -z ${1+x} ]
then
	echo "./new_deploy <deploy_name>"
	exit 1
fi
deploy_name=$1
deploy_path="./deploys/$deploy_name"

cd $deploy_path
rm -f pids
. ./export_config.sh
printenv
./indexserver & 
echo $! >> pids

./forwardindexserver & 
echo $! >> pids

./forcerankingserver texts_config & 
echo $! >> pids
sleep 1

./rankingserver texts_config & 
echo $! >> pids
sleep 1

./southserver & 
echo $! >> pids
sleep 1

python stem_server.py & 
echo $! >> pids
echo "Servers are done"




