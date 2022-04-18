#!/usr/bin/bash

[[ -f $1 ]] || exit 1
rm -rf test-tmp || exit 1
mkdir test-tmp || exit 1

failed=0
for inputfile in test-data/*.in; do
	f=${inputfile##*/}
	./$1 $inputfile test-tmp/$f.out
	cmp ${inputfile}.out test-tmp/$f.out
	if [[ $? -ne 0 ]]; then
		echo "TEST $f FAILED"
		diff -u test-data/$f.out test-tmp/$f.out
		failed=$(($failed + 1))
	fi
done

if [[ $failed -eq 0 ]]; then
	echo "ALL PASSED"
	exit 0
else
	echo "$failed FAILURES"
	exit 1
fi