#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <random>

// works for 2d matrices only
//
// format is the following
//
// rows cols
// matrix data
// row cols
// matrix data
// row cols
// ...

const int sigfigs = 3;

const double small_mean = 0;
const double large_mean = 0.5;
const double small_std_dev = 0.1;
const double large_std_dev = 0.3;
std::default_random_engine gen;
std::normal_distribution<double> lowDist(small_mean, small_std_dev);
std::normal_distribution<double> highDist(large_mean, large_std_dev);

const double sparsity = 0.9;
const int mat_num = 100;
const int input_mat_rows = 10;
const int input_mat_cols = 10;
const int kernel_mat_rows = 3;
const int kernel_mat_cols = 3;

// returns a number between 0 and 1 from a bimodal distribution
double randGen() {
	double num = 2;
	while (num > 1 || num < 0) {
		if (rand() % 1000 < sparsity*1000) {
			num = lowDist(gen);
		} else {
			num = highDist(gen);
		}
	}
	return num;
}

int main() {
	
	std::ofstream out;
	out.open("data.out");
	
	srand(time(NULL));

	for (int mat_i = 0; mat_i < mat_num; ++mat_i) {
		int mat_rows = mat_i % 2 ? input_mat_rows : kernel_mat_rows;
		int mat_cols = mat_i % 2 ? input_mat_cols : kernel_mat_cols;

		out << mat_rows << " " << mat_cols << std::endl;
		for (int i = 0; i < mat_rows*mat_cols; ++i) {
			out << randGen();
			if (i != mat_rows*mat_cols - 1)
				out << " ";	
		}
		out << std::endl;
	}

	return 0;
}
