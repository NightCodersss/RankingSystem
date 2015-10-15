DOCS_PATH="/home/asio/pressa/mnt/backup2/serg/pressa/A"

words="города бюджет газета день событие яблоко музыка страна плохо осень погибла"
files=$(find $DOCS_PATH -iname '*.txt')

i=0

rm file_ids.dat
rm *.index

for file in $files
do
	((i++))
	echo "$i $file" >> file_ids.dat

#	echo $file

	for word in $words
	do
		count=$(grep -w -c "$word" $file)
		if [ $count -ne 0 ]; then
			all=$(./wordcount.py $file | awk '{print $1}')
#			echo "all = $all"

			corr=$(echo "scale=10; $count / $all" | bc)
			echo "$word $i $corr" >> "$word.index"
		fi		 
	done
done
