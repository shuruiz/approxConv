#!/bin/bash
#!/bin/bash

ROWS=10
COLS=10
SIZE=100

	for s in $(seq 0 $SIZE); do
		cp dataconv.c data.c
		
		SPARSITY=$s
		NUMSPARSE=0
		echo $SPARSITY
		for i in $(seq 1 $ROWS);
		do
			for ((j=1;j<=COLS;j++)); do
				if [ "$NUMSPARSE" -lt "$SPARSITY" ]; then
    				sed -i s/@$i-$j/10/ data.c;
				else
					sed -i s/@$i-$j/100/ data.c;
				fi
				NUMSPARSE=$((NUMSPARSE + 1));
			done
		done

		cat data.c


		PATH_MYPASS=~/projects/583project/build/convpass/LLVMCONV.so ### Action Required: Specify the path to your pass ###
		NAME_MYPASS=-convpass ### Action Required: Specify the name for your pass ###
		BENCH=data.c

		clang -emit-llvm -S ${BENCH} -o - | sed s/optnone// | opt -load ${PATH_MYPASS} -convpass -mem2reg -sccp -sroa -sccp -threshpass -mem2reg -sroa -mem2reg -die -dse -adce -sccp > opt.bc 2> /dev/null 

		clang opt.bc -o optimized
		time ./optimized < ../convolution/inputbig.txt
		#chmod +x optimized

		#time ./optimized < ../convolution/inputbig.txt

		
		clang -emit-llvm -S ${BENCH} -o - | sed s/optnone// | opt -load ${PATH_MYPASS} -mem2reg -sccp -sroa -sccp -mem2reg -sroa -mem2reg -die -dse -adce -sccp -O1 > opt.bc 2> /dev/null 
	
	
		clang opt.bc -o optimized
		time ./optimized < ../convolution/inputbig.txt
	
	done
