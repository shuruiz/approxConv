#include <stdio.h>

#define IH 1000
#define IW 1000

#define KH 10
#define KW 10

const int outH = IH-KH+1;
const int outW = IW-KW+1;

void testloops(float* input, float* output) {
    
	
	
const float array[10][10] ={
				          { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				          { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				          { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				          { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				          { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				          { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				          { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				          { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				          { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				          { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
						  };
	
    for(int oh=0; oh<outH; oh++) {
        for (int ow=0; ow<outW; ow++) {
          for(int kh=0; kh<KH; kh++){
            for(int kw=0; kw<KW; kw++){
	        output[(oh*outW) + ow] += (float)input[(oh+kh)*IW + ow+kw] * array[kh][kw];
	    }
	  }
	}
    }
}





int main(){
    float input[IH*IW];
    float output[outH*outW];
    for (int i = 0; i < IH*IW; ++i) {
        scanf("%f", input + i);
    }

    testloops(input, output);
    	
	for (int i=0; i < 996*996; ++i) {
    	fprintf(stdout, "%f ", output[i]);	
	}
	fprintf(stdout, "\n");


	
	//int tot=0;
    //for(int i=0; i<10; i++) {
    //    //tot+=1;
    //}
    return 0;
}
