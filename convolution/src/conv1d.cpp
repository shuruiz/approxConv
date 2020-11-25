// conv1d.cpp
// 583project
#include <stdio.h>
#include <vector>




std::vector<double> input = {1, 2, 0, 2, 1, 1, 2, 0, 2, 1};
std::vector<double> kernel = {1, 2, 0, 2, 1};
std::vector<double> output = {0, 0, 0, 0, 0, 0};

void conv1d(std::vector<double> const &in, std::vector<double> const &k, std::vector<double> &out){
    for(int i=0; i<(in.size()-k.size()+1); i++) {
        for(int j=0; j<k.size(); j++) {
            out[i] += in[i+j] * k[j];
	}
    }
}


int main(){
    conv1d(input, kernel, output);
    
    for(int i=0; i<output.size(); i++) {
        fprintf(stdout, "%f ", output[i]);
    }
    fprintf(stdout, "\n");

    return 0;
}
