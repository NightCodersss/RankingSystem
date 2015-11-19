rm -f indexlist

for f in *.index
do
	sort -t ' ' -k 3 -n -r $f > "sorted_$f"
	echo "sorted_$f" >> indexlist
done
