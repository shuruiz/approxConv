#!/bin/bash
#!/bin/bash

# make output data file
echo  %sparsity,runtime,baseline > sparsityruntime.out

ROWS=10
COLS=10
SIZE=100

	for s in $(seq 0 $SIZE); do
		cp dataconv.c data.c
		
		SPARSITY=$s
		NUMSPARSE=0
		for i in $(seq 1 $ROWS);
		do
			for ((j=1;j<=COLS;j++)); do
				if [ "$NUMSPARSE" -lt "$SPARSITY" ]; then
    				sed -i s/@$i-$j/1/ data.c;
				else
					sed -i s/@$i-$j/1000/ data.c;
				fi
				NUMSPARSE=$((NUMSPARSE + 1));
			done
		done

		#cat data.c


		PATH_MYPASS=~/projects/583project/build/convpass/LLVMCONV.so ### Action Required: Specify the path to your pass ###
		NAME_MYPASS=-convpass ### Action Required: Specify the name for your pass ###
		BENCH=data.c

		clang -emit-llvm -S ${BENCH} -o - | sed s/optnone// | opt -load ${PATH_MYPASS} -convpass -mem2reg -sccp -sroa -sccp -threshpass -mem2reg -sroa -mem2reg -die -dse -adce -sccp -O1 > opt.bc 2> /dev/null 

		clang opt.bc -o optimized

		echo -n "$SPARSITY," >> sparsityruntime.out
		{ time ./optimized < ../convolution/inputbig.txt > /dev/null; } 2>&1 | grep real | awk '{print $2}' | cut -c 3- | cut -c -5 | xargs echo -n >> sparsityruntime.out

		#chmod +x optimized

		#time ./optimized < ../convolution/inputbig.txt

		
		clang -emit-llvm -S ${BENCH} -o - | sed s/optnone// | opt -load ${PATH_MYPASS} -mem2reg -sccp -sroa -sccp -mem2reg -sroa -mem2reg -die -dse -adce -sccp -O1 > opt1.bc 2> /dev/null 
	
		echo -n , >> sparsityruntime.out
		clang opt1.bc -o optimized
		#time ./optimized < ../convolution/inputbig.txt
		{ time ./optimized < ../convolution/inputbig.txt > /dev/null; } 2>&1 | grep real | awk '{print $2}' | cut -c 3- | cut -c -5 >> sparsityruntime.out

	done
