#!/bin/bash
### run.sh
### benchmark runner script
### Locate this script at each benchmark directory. e.g, 583simple/run.sh
### usage: ./run.sh ${benchmark_name} ${input} 
### e.g., ./run.sh compress compress.in or ./run.sh simple

PATH_MYPASS=~/projects/583project/build/convpass/LLVMCONV.so ### Action Required: Specify the path to your pass ###
NAME_MYPASS=-convpass ### Action Required: Specify the name for your pass ###
BENCH=src/${1}.c
INPUT=${2}

setup(){
if [[ ! -z "${INPUT}" ]]; then
echo "INPUT:${INPUT}"
ln -sf input1/${INPUT} .
fi
}


rm *.bc
rm *.profraw
rm *.prof*

# Prepare input to run
setup
# Convert source code to bitcode (IR)
# This approach has an issue with -O2, so we are going to stick with default optimization level (-O0)
clang -emit-llvm -c ${BENCH} -o ${1}.bc

# Canonicalize natural loops
#clang -emit-llvm -c ${BENCH} -o - | sed s/optnone// | opt -analyze -mem2reg -loop-rotate -loops -indvars -instcombine -loop-simplify -loop-rotate -scalar-evolution

opt -mem2reg -indvars -loop-simplify -scalar-evolution ${1}.bc -o ${1}.ls.bc
#opt -analyze -mem2reg -indvars -instcombine -loop-simplify -loop-rotate -scalar-evolution < ${1}.bc
# Instrument profiler
opt -pgo-instr-gen -instrprof ${1}.ls.bc -o ${1}.ls.prof.bc
# Generate binary executable with profiler embedded
clang -fprofile-instr-generate ${1}.ls.prof.bc -o ${1}.prof
# Collect profiling data
./${1}.prof ${INPUT}
# Translate raw profiling data into LLVM data format
llvm-profdata merge -output=${1}.profdata default.profraw

# Prepare input to run
setup
# Apply your pass to bitcode (IR)
opt -pgo-instr-use -pgo-test-profile-file=${1}.profdata -load ${PATH_MYPASS} ${NAME_MYPASS} < ${1}.ls.bc > ${1}.convpass.bc



../viz.sh ${1}.ls
../viz.sh ${1}.convpass

