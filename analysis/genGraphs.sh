
		PATH_MYPASS=~/projects/583project/build/convpass/LLVMCONV.so ### Action Required: Specify the path to your pass ###
		NAME_MYPASS=-convpass ### Action Required: Specify the name for your pass ###
		BENCH=dataconvgen.c

		sed -n 18,22p dataconvgen.c		
		
		clang -emit-llvm -S ${BENCH} -o - | sed s/optnone// | opt -load ${PATH_MYPASS} > opt1.bc 2> /dev/null 
		clang -emit-llvm -S ${BENCH} -o - | sed s/optnone// | opt -load ${PATH_MYPASS} -convpass > opt2.bc 2> /dev/null 
		clang -emit-llvm -S ${BENCH} -o - | sed s/optnone// | opt -load ${PATH_MYPASS} -convpass -mem2reg -sccp -sroa -sccp > opt3.bc 2> /dev/null 
		clang -emit-llvm -S ${BENCH} -o - | sed s/optnone// | opt -load ${PATH_MYPASS} -convpass -mem2reg -sccp -sroa -sccp -threshpass > opt4.bc 2> /dev/null 
		clang -emit-llvm -S ${BENCH} -o - | sed s/optnone// | opt -load ${PATH_MYPASS} -convpass -mem2reg -sccp -sroa -sccp -threshpass -mem2reg -sroa -mem2reg -die -dse -adce -sccp -O1 > opt5.bc 2> /dev/null 

        
		bash ../viz.sh opt1
		bash ../viz.sh opt2
		bash ../viz.sh opt3
		bash ../viz.sh opt4
		bash ../viz.sh opt5

		clang opt5.bc -o optimized
		./optimized < ../convolution/inputbig.txt > out5.txt
		clang opt1.bc -o optimized
		./optimized < ../convolution/inputbig.txt > out1.txt
		#diff out5.txt out1.txt
	
	
