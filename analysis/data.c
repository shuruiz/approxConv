#include <stdio.h>




void testloops(float* input) {
    
	
	
const float array[5][5] ={{ 10, 10, 10, 100, 100},
				          { 100, 100, 100, 100, 100},
				          { 100, 100, 100, 100, 100},
				          { 100, 100, 100, 100, 100},
				          { 100, 100, 100, 100, 100}};
	
	
	float tot = 0;
    for(int i=0; i<100; i++) {
        for (int j=0; j<100; j++) {
          for(int k=0; k<5; k++){
            for(int l=0; l<5; l++){
				
			  tot+= (float)input[k*5 + l] * array[k][l];
	        }
	      }
	    }
    }
    fprintf(stdout, "%f\n", tot);
}





int main(){
	float input[25];
	for (int i = 0; i < 25; ++i) {
    	scanf("%f", input + i);
	}

    testloops(input);
    //int tot=0;
    //for(int i=0; i<10; i++) {
    //    //tot+=1;
    //}
    return 0;
}
