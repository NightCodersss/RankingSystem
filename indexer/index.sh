DOC=$1

echo "Indexing " $(DOC)
WORK_DIR=`mktemp -d`
SCRIPT_DIR=`pwd`
COMMIT_FILE=$SCRIPT_DIR/commit

cd WORK_DIR
python "$SCRIPT_DIR/mine_texts.py" "$DOC"
for TEXT_FILE in `ls *.text`
do
	python "$SCRIPT_DIR/index_text.py" $TEXT_FILE $COMMIT_FILE
done
