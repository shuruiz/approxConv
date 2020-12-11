#include <stdio.h>




void testloops(double* input) {
    
	
	
const double array[5][5] ={{  1,   2, 3, 4,  5},
				{6,   7, 8, 9,    10},
				{54,   5, 346, 64, 74},
				{54, 54, 54,   125,  125},
				{1, 125, 125,   125, 125}};
	
	
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
