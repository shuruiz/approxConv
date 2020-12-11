#include <stdio.h>


double array[5][5] ={{  0.1,   0.1, 0.1, 0.1,   0.1},
				{0.1,   0.11, 0.1, 0.1,    0.1},
				{0.1,   0.11, 0.1, 0.1, 0.01},
				{0.1, 0.1, 0.1,   0.1,    0.1},
				{  0.1, 0.1,   0.1,   0.1, 0.01}};


void testloops(double* input) {
    double tot = 0;
    for(int i=0; i<100; i++) {
        for (int j=0; j<100; j++) {
          for(int k=0; k<5; k++){
            for(int l=0; l<5; l++){
				
			  tot+= (double)input[k*5 + l] * array[k][l];
	        }
	      }
	    }
    }
    fprintf(stdout, "%lf\n", tot);
}





int main(){
	double input[25];
	for (int i = 0; i < 25; ++i) {
    	scanf("%lf", input + i);
	}

    testloops(input);
    //int tot=0;
    //for(int i=0; i<10; i++) {
    //    //tot+=1;
    //}
    return 0;
}
