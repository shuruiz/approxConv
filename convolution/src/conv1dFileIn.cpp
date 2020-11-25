// conv1d.cpp
// 583project
#include <stdio.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>


void conv1d(std::vector<double> const &in, std::vector<double> const &k, std::vector<double> &out){
    for(int i=0; i<(in.size()-k.size()+1); i++) {
        out.push_back(0);
	for(int j=0; j<k.size(); j++) {
            out[i] += in[i+j] * k[j];
	}
    }
}


int main(){
    std::string matString;
    std::string kernelString;
    while (getline(std::cin, matString) && getline(std::cin, kernelString)) {
	
        std::stringstream matStream(matString);
        std::vector<int> input;
	int element;
        while (matStream >> element)
            input.push_back(element);

	std::stringstream kernelStream(kernelString)
	std::vector<int> kernel;
	while (kernelStream >> element)
	    kernel.push_back(element);
        
	std::vector<int> output;
    	conv1d(input, kernel, output);
    
        for(int i=0; i<output.size(); i++) {
        	fprintf(stdout, "%f ", output[i]);
    	}
    	fprintf(stdout, "\n");

    }
    return 0;
}
