
if [ -z "$MAIN_EXPORTS_ARE_DONE" ] 
then
	echo "Make common exports";
	exit 1;
fi
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$RANKING_SYSTEM_HOME/UbjsonCpp/src/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$RANKING_SYSTEM_HOME/config_loader/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$RANKING_SYSTEM_HOME/common/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$RANKING_SYSTEM_HOME/storage/

export PYTHONPATH=$PYTHONPATH:$RANKING_SYSTEM_HOME/common/

export INDEX_PATH=$RANKING_SYSTEM_HOME/storage/index/
export FORWARD_INDEX_PATH=$RANKING_SYSTEM_HOME/indexer/
export IDF_PATH=$RANKING_SYSTEM_HOME/indexer/idf

export SOUTH_PORT=15000
export RANKING_PORT=16000
export INDEX_PORT=14000
export FORWARD_PORT=17000
export FORCE_RANKING_PORT=18000
export STEM_PORT=19000
export RANKING_SERVER_LOGGING=NO

export MAIN_EXPORTS_ARE_DONE=1
