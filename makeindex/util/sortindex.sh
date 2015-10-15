for f in *.index
do
	sort -t ' ' -k 3 -n -r $f > "sorted_$f"
done
