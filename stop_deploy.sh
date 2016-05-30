if [ -z ${1+x} ]
then
	echo "./new_deploy <deploy_name>"
	exit 1
fi
deploy_name=$1
deploy_path="./deploys/$deploy_name"

cd $deploy_path

cat pids | while read pid
do
	kill $pid
done
rm -f pids
