DOCS_PATH="/home/asio/pressa/mnt/backup2/serg/pressa/A/"

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

	n=100
	arg=$(printf "1,%sd" "$n")

	for word in $words
	do		
		count1=$(head -$n $file | grep -w -c "$word")
		count2=$(cat "$file" | sed "$arg" | grep -w -c "$word")
		if [ $(echo "$count1 + $count2" | bc) -ne 0 ]; then
			head -$n $file > $file.tmp1
			echo $arg
			cat "$file" | sed "$arg" > $file.tmp2
			all1=$(./wordcount.py $file.tmp1 | awk '{print $1}')
			all2=$(./wordcount.py $file.tmp2 | awk '{print $1}')

			if [ $all1 -ne 0 ]; then
				corr1=$(echo "scale=10; $count1 / $all1" | bc)
				echo "$word $i $corr1" >> "$word.1.index"
			fi				
			if [ $all2 -ne 0 ]; then
				corr2=$(echo "scale=10; $count2 / $all2" | bc)
				echo "$word $i $corr2" >> "$word.2.index"
			fi

			rm $file.tmp1 $file.tmp2
		fi		 
	done
done
