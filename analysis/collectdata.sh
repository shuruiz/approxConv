#!/bin/bash
cp datatemplate.c data.c

ROWS=5
COLS=5

SPARSITY=3


NUMSPARSE=0
for i in $(seq 1 $ROWS);
do
	for ((j=1;j<=COLS;j++)); do
		if [ "$NUMSPARSE" -lt "$SPARSITY" ]; then
    		sed -i s/@$i$j/10/ data.c;
		else
			sed -i s/@$i$j/100/ data.c;
		fi
		NUMSPARSE=$((NUMSPARSE + 1));
	done
done
