
// conv2d.cpp
// 583project
#include <stdio.h>


#define KH 5
#define KW 5


#define IH 1000
#define IW 1000

#define OH 9996
#define OW 9996

double kernel[KH*KW] = {0, 0, 0, 0, 0,
			0, 0, 0, 0, 0,
			0, 0, 1, 0, 0,
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0};
double input[IH*IW] = { 1 };
double output[OH*OW] = { 0 };
/*
void conv2d(double in[], int inh, int inw, 
	    double k[], double out[]){
    for(int ih=0; ih<(inh-KH+1); ih++) {
        for(int iw=0; iw<(inw-KW+1); iw++) {
            for(int jh=0; jh<5; jh++){
                for(int jw=0; jw<5; jw++) {
                    out[(ih*IW)+iw] += in[((ih+jh)*IW)+iw+jw] * k[(jh*KH)+jw];	
		}
	    }
        }
    }
}*/

/*
void createMatrix(std::vector<double>& mat, std::istringstream mstream, int rows, int cols) {
		if (!mat.empty()) std::err << "Must input empty mat\n";
		
		std::vector<double> temp(cols, 0);
		mat.resize(rows, temp);
		
		double element;
		for (int i = 0; mstream >> element; ++i) {
			mat[i / rows][i % rows] = element;
		}
}
*/
// odd numbered matrix inputs are kernels and even numbers are
// input matrices
int main() {	
	
	/*
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
	}*/


	//conv2d(input, 500, 500, kernel, output);
	for(int i=0; i<50; i++) {
            for(int j=0; j<50; j++) {
                fprintf(stdout, "%f ", output[(i*OW)+j]);
            }
            fprintf(stdout, "\n");
        }
    return 0;
}

