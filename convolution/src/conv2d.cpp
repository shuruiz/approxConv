// conv1d.cpp
// 583project
#include <stdio.h>
#include <vector>




std::vector<std::vector<double>> input = {
	{1, 2, 0, 2, 1, 1, 2, 0, 2, 1},
	{2, 0, 2, 1, 1, 2, 0, 2, 1, 2},
	{0, 2, 1, 1, 2, 0, 2, 1, 2, 0},
	{2, 1, 1, 2, 0, 2, 1, 2, 0, 2},
	{1, 1, 2, 0, 2, 1, 2, 0, 2, 1},
	{1, 2, 0, 2, 1, 2, 0, 2, 1, 1},
	{2, 0, 2, 1, 2, 0, 2, 1, 1, 2},
	{0, 2, 1, 2, 0, 2, 1, 1, 2, 0},
	{2, 1, 2, 0, 2, 1, 1, 2, 0, 2},
	{1, 2, 0, 2, 1, 1, 2, 0, 2, 1}
};
std::vector<std::vector<double>> kernel = {
	{1, 2, 0, 2, 1},
	{2, 0, 2, 1, 1},
	{0, 2, 1, 1, 2},
	{2, 1, 1, 2, 0},
	{1, 1, 2, 0, 2}
};
std::vector<std::vector<double>> output = {
	{0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0}
};

void conv1d(std::vector<std::vector<double>> const &in, 
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


int main(){
    conv1d(input, kernel, output);

    for(int i=0; i<output.size(); i++) {
	for(int j=0; j<output.size(); j++) {
            fprintf(stdout, "%f ", output[i][j]);
        }
	fprintf(stdout, "\n");
    }

    return 0;
}

