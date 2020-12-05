// conv2d.cpp
// 583project
#include <stdio.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>

void conv2d(std::vector<std::vector<double>> const &in, 
		std::vector<std::vector<double>> const &k, 
		std::vector<std::vector<double>> &out){
    for(int ih=0; ih<(in.size()-k.size()+1); ih++) {
        for(int iw=0; iw<(in[0].size()-k[0].size()+1); iw++) {
            for(int jh=0; jh<k.size(); jh++){
                for(int jw=0; jw<k[0].size(); jw++) {
                    	out[ih][iw] += in[ih+jh][iw+jw] * k[jh][jw];	
				}
	    	}
        }
    }
}

void createMatrix(std::vector<double>& mat, std::istringstream mstream, int rows, int cols) {
		if (!mat.empty()) std::err << "Must input empty mat\n";
		
		std::vector<double> temp(cols, 0);
		mat.resize(rows, temp);
		
		double element;
		for (int i = 0; mstream >> element; ++i) {
			mat[i / rows][i % rows] = element;
		}
}

// odd numbered matrix inputs are kernels and even numbers are
// input matrices
int main() {	
	
	int krows, kcols;
    while ( std::cin >> krows >> kcols) {	
		std::string kstring;
		std::getline(std::cin, kstring);
		createMatrix(kernel, kstring, krows, kcols);

		int irows, icols;
		std::cin >> irows >> icols;
		std::getline(std::cin, istring);
		createMatrix(input, istring, irows, icols);
		
		int orows = irows - krows + 1;
		int ocols - icols - kcols + 1;
		std::vector<double> otemp(ocols, 0);
		std::vector< std::vector<double> > output(orows, otemp);

		conv2d(input, kernel, output); 

		for(int i=0; i<output.size(); i++) {
	    	for(int j=0; j<output[0].size(); j++) {
				fprintf(stdout, "%f ", output[i][j]);
	    	}
	    	fprintf(stdout, "\n");
		}
	}

    return 0;
}

