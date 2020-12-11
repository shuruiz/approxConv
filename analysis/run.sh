#!/bin/bash
#!/bin/bash
cp dataconv.c data.c

ROWS=5
COLS=5
SIZE=25

	for s in $(seq 0 $SIZE); do

		SPARSITY=$s
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




		PATH_MYPASS=~/projects/583project/build/convpass/LLVMCONV.so ### Action Required: Specify the path to your pass ###
		NAME_MYPASS=-convpass ### Action Required: Specify the name for your pass ###
		BENCH=data.c

		clang -emit-llvm -S ${BENCH} -o - | sed s/optnone// | opt -load ${PATH_MYPASS} -convpass -mem2reg -sccp -sroa -sccp -threshpass -mem2reg -sroa -mem2reg -die -dse -adce -sccp > opt.bc 2> /dev/null 

		clang opt.bc -o optimized
		time ./optimized < ../convolution/inputbig.txt
		#chmod +x optimized

		#time ./optimized < ../convolution/inputbig.txt
	done
