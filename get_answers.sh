out_dir=$1
cat queries.txt | while read line
do
	echo "$line"
	echo "$line" | nc localhost 15000 > $out_dir/"$line".log
done
