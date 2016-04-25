DOC=$1
DOC_ID=`echo $DOC | md5sum | sed "s/ *- *//"`

echo "Indexing " $DOC
echo "Doc_id: " $DOC_ID

WORK_DIR=`mktemp -d`
echo "Work dir: " $WORK_DIR

SCRIPT_DIR=`pwd`
COMMIT_FILE=$SCRIPT_DIR/commit

cd $WORK_DIR
python "$SCRIPT_DIR/mine_texts.py" "$DOC"
for TEXT_FILE in `ls *.text`
do
	TEXT_ID=${TEXT_FILE%.text}
	python "$SCRIPT_DIR/index_text.py" $TEXT_ID $DOC_ID $COMMIT_FILE
done

rm -rf $WORK_DIR
