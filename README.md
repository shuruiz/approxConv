# Compile Time Approximate Convolution

This project is an attempt to optimize sparse-kernel convolutions at compile time by removing computations involving elements of the kernel that are below a certain threshold.
This method results in a significant speedup while introducing a very small amount of error.

Running the script analysis/demo.sh produces CFGs in the dot folder that relate to each step of the optimization procedure.

1 - The unoptimized IR

2 - IR after targeted loop unrolling has been applied

3 - IR after subsequent "stock" llvm passes have been applied to propogate the kernel values into the IR as constants

4 - IR after the thresholding pass has been applied to remove all computations relating to kernel values below the threshold

5 - Final IR after dead code elimination and subsequent optimizations



Running the script analysis/run.sh will run the optimizations on kernels of various sparisities.  The resulting runtime data comparing the baseline computation to the optimized computation is printed to the file sparsityruntime.out

Code can be found within the convpass/ and sample convolution code that is used for the above scripts can be seen in analysis/dataconv.c.  This is the code that is used to insert kernels of various sparsity to analyze runtime.
